#!/usr/bin/bash
# strict mode
set -e 
# taken from https://tools.ietf.org/html/rfc4648
TEST_VECTORS=("" "f" "fo" "foo" "foob" "fooba" "foobar")
echo "============="
echo "Base 64 Tests"
echo "============="
for v in ${TEST_VECTORS[@]}; do
    echo "BASE64($v) =  $(./base64 -e -s $v)";
done

echo "============="
echo "Base 16 Tests"
echo "============="
for v in ${TEST_VECTORS[@]}; do
    echo "BASE64($v) =  $(./base64 -e -h -s $v)";
done
