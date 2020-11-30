#!/usr/bin/bash
# strict mode
set -e 
# taken from https://tools.ietf.org/html/rfc4648
TEST_ENCODE=("" "f" "fo" "foo" "foob" "fooba" "foobar")
TEST_DECODE=("" "Zg==" "Zm8=" "Zm9v" "Zm9vYg==" "Zm9vYmE=" "Zm9vYmFy")
echo "============="
echo "Base 64 Tests"
echo "============="
for v in ${TEST_ENCODE[@]}; do
    echo "BASE64($v) =  $(./base64 encode $v)";
done
for v in ${TEST_DECODE[@]}; do
    echo "BASE64($v) =  $(./base64 decode $v)";
done
