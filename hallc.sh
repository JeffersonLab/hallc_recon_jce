#!/usr/bin/env bash
# Run from the hallc_recon_jce checkout:
#   source hallc.sh build /path/to/ROOT  # Build, install, and set up the environment.
#   source hallc.sh        # Set up an existing installation only.
# Optionally export JCE_SOURCE_DIR=/path/to/jana2-common-extensions first.

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
    echo "Source this script so it can configure your shell: source hallc.sh [build ROOT_PREFIX]"
    exit 2
fi

_hallc_setup() {
    local root="$PWD"
    local jce_source="${JCE_SOURCE_DIR-${root}/../jana2-common-extensions}"

    if [[ -z "$jce_source" ]]; then
        echo "JCE_SOURCE_DIR is empty. Set it to an existing JCE checkout or unset JCE_SOURCE_DIR."
        return 1
    fi
    [[ "$jce_source" == /* ]] || jce_source="${root}/${jce_source}"
    if [[ ${JCE_SOURCE_DIR+x} && ! -f "$jce_source/superbuild/CMakeLists.txt" ]]; then
        echo "JCE_SOURCE_DIR must point to an existing JCE checkout with superbuild/CMakeLists.txt."
        return 1
    fi
    if (( $# > 2 )) || [[ $# -gt 0 && "$1" != build ]]; then
        echo "Usage: source hallc.sh [build ROOT_PREFIX]"
        return 1
    fi
    if [[ "${1:-}" == build && ( $# -ne 2 || -z "$2" ) ]]; then
        echo "ROOT prefix is required: source hallc.sh build /path/to/ROOT (or source hallc.csh build /path/to/ROOT)."
        return 1
    fi
    if [[ ! -f "$root/hallc.sh" || ! -f "$root/CMakeLists.txt" ]]; then
        echo "Run this command from your hallc_recon_jce checkout."
        return 1
    fi

    if [[ "${1:-}" == build ]]; then
        local root_config="" candidate
        for candidate in "$2/cmake" "$2/lib/cmake/ROOT" "$2/lib64/cmake/ROOT" "$2"; do
            if [[ -f "$candidate/ROOTConfig.cmake" ]]; then
                root_config=$(cd -- "$candidate" && pwd) || return 1
                break
            fi
        done
        if [[ -z "$root_config" ]]; then
            echo "ROOTConfig.cmake not found under the supplied ROOT prefix: $2"
            return 1
        fi
        local prefixes="${CMAKE_PREFIX_PATH//:/;}"
        prefixes="$2${prefixes:+;$prefixes}"

        if [[ ! -d "$jce_source" ]]; then
            git clone https://github.com/JeffersonLab/jana2-common-extensions.git "$jce_source" || return 1
        fi
        cmake -S "$jce_source/superbuild" -B "$jce_source/build-super" \
            -DCMAKE_INSTALL_PREFIX="$jce_source/jce-stack" || return 1
        cmake --build "$jce_source/build-super" --parallel || return 1
        cmake -S "$root" -B "$root/build" \
            -DCMAKE_PREFIX_PATH="$jce_source/jce-stack;$prefixes" \
            -DROOT_DIR="$root_config" \
            -DCMAKE_INSTALL_PREFIX="$root" || return 1
        cmake --build "$root/build" --parallel || return 1
        cmake --install "$root/build" || return 1
    fi

    if [[ ! -x "$jce_source/jce-stack/bin/jana" || ! -d "$root/lib/plugins" ]]; then
        echo "JCE or Hall C is not installed yet. Run: source hallc.sh build /path/to/ROOT"
        return 1
    fi
    export JCE_HOME="$jce_source/jce-stack"
    export HALLC_RECON_HOME="$root"
    case ":${JANA_PLUGIN_PATH-}:" in
        *":${root}/lib/plugins:"*) ;;
        *) export JANA_PLUGIN_PATH="$root/lib/plugins${JANA_PLUGIN_PATH:+:$JANA_PLUGIN_PATH}" ;;
    esac
}

if _hallc_setup "$@"; then
    unset -f _hallc_setup
    return 0
else
    unset -f _hallc_setup
    return 1
fi
