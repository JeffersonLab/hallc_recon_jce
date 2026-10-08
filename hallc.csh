# tcsh wrapper for the primary Bash helper. Source from the Hall C checkout:
#   source hallc.csh [build ROOT_PREFIX]
set _hallc_ok = 0
set _hallc_env = "`mktemp`"
if ($status != 0) goto hallc_done

# Keep build output visible; pass only the resulting environment through a file.
bash -c 'env_file=$1; shift; source ./hallc.sh "$@" || exit $?; printf "%s\n" "$JCE_HOME" "$HALLC_RECON_HOME" "$JANA_PLUGIN_PATH" > "$env_file"' hallc "$_hallc_env" $argv:q
if ($status == 0) then
    setenv JCE_HOME "`sed -n '1p' "$_hallc_env"`"
    setenv HALLC_RECON_HOME "`sed -n '2p' "$_hallc_env"`"
    setenv JANA_PLUGIN_PATH "`sed -n '3p' "$_hallc_env"`"
    set _hallc_ok = 1
endif
/bin/rm -f "$_hallc_env"

hallc_done:
unset _hallc_env
# Return failure without exiting the shell that sourced this wrapper.
eval "unset _hallc_ok; /bin/test $_hallc_ok = 1"
