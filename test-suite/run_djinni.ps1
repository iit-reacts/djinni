param (
    [switch]$clean = $false
)

$base_dir = $PSScriptRoot
$djinni_dir = "$(Split-Path -Parent $base_dir)\src"

$temp_out = "$base_dir\djinni-output-temp"
$in = "$base_dir\djinni\py_all.djinni"
#$wchar_in = "$base_dir\djinni\wchar_test.djinni"

$temp_out_relative = "djinni-output-temp"
$in_relative = "djinni\py_all.djinni"
#$wchar_in_relative = "djinni\wchar_test.djinni"

$cpp_out = "$base_dir\generated-src\cpp"
$cs_out = "$base_dir\generated-src\cs"
$cppcli_out = "$base_dir\generated-src\cppcli"

$gen_stamp = "$temp_out\gen.stamp"

$deps_file = "$temp_out\$djinnifile.deps"

echo "base_dir = $base_dir"
echo "djinni_dir = $djinni_dir"

echo "temp_out = $temp_out"
echo "in = $in"
#echo "wchar_in = $wchar_in"

echo "temp_out_relative = $temp_out_relative"
echo "in_relative = $in_relative"
#echo "wchar_in_relative = $wchar_in_relative"

echo "cpp_out = $cpp_out"
echo "cs_out = $cs_out"
echo "cppcli_out = $cppcli_out"

echo "gen_stamp = $gen_stamp"

echo "deps_file = $deps_file"

if ($clean) {
    # TODO!
    echo "cleaning..."
    
    exit
}

# Build djinni
bash.exe $djinni_dir\build

# TODO check if it was already generated...

if (Test-Path $temp_out) {
    # Directory exists.
    Remove-Item -Recurse -Force $temp_out
}

cd $base_dir

# Run Djinni generation
#& "$base_dir\..\src\target\start.bat" `
#    --cpp-out "$temp_out_relative/cpp" `
#    --cpp-namespace testsuite `
#    --ident-cpp-enum-type foo_bar `
#    --cpp-optional-template "std::experimental::optional" `
#    --cpp-optional-header "`"../../handwritten-src/cpp/optional.hpp`"" `
#    --cpp-extended-record-include-prefix "../../handwritten-src/cpp/" `
#    --cpp-use-wide-strings true `
#    `
#    --cs-out "$temp_out/cs" `
#    --cs-namespace Djinni.TextSort `
#    --cppcli-out "$temp_out/cppcli" `
#    `
#    --idl "$wchar_in_relative"


& "$base_dir\..\src\target\start.bat" `
    --cpp-out "$temp_out_relative/cpp" `
    --cpp-namespace testsuite `
    --ident-cpp-enum-type foo_bar `
    --cpp-optional-template "std::experimental::optional" `
    --cpp-optional-header "`"../../handwritten-src/cpp/optional.hpp`"" `
    --cpp-extended-record-include-prefix "../../handwritten-src/cpp/" `
    `
    --cs-out "$temp_out/cs" `
    --cs-namespace Djinni.TextSort `
    --cppcli-out "$temp_out/cppcli" `
    `
    --idl "$in_relative" `
    --idl-include-path "djinni/vendor"
