param (
    [switch]$clean = $false
)

$base_dir = $PSScriptRoot
$djinni_dir = "$(Split-Path -Parent $base_dir)\src"

$temp_out = "$base_dir\djinni-output-temp"
$in = "$base_dir\djinni\all.djinni"
$wchar_in = "$base_dir\djinni\wchar_test.djinni"

$temp_out_relative = "djinni-output-temp"
$in_relative = "djinni\all.djinni"
$wchar_in_relative = "djinni\wchar_test.djinni"

$cpp_out = "$base_dir\generated-src\cpp"
$cs_out = "$base_dir\generated-src\cs"
$cppcli_out = "$base_dir\generated-src\cppcli"

$gen_stamp = "$temp_out\gen.stamp"

$deps_file = "$temp_out\$djinnifile.deps"

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
& "$base_dir\..\src\target\start.bat" `
    --cpp-out "$temp_out_relative/cpp" `
    --cpp-namespace testsuite `
    --ident-cpp-enum-type foo_bar `
    --cpp-optional-template "std::experimental::optional" `
    --cpp-optional-header "\`"../../handwritten-src/cpp/optional.hpp\`"" `
    --cpp-extended-record-include-prefix "../../handwritten-src/cpp/" `
    --cpp-use-wide-strings true `
    `
    --cs-namespace Djinni.TestSuite `
    --cppcli-out "$temp_out_relative/cppcli" `
    `
    --yaml-out "$temp_out_relative/yaml" `
    --yaml-out-file "yaml-test.yaml" `
    --yaml-prefix "test_" `
    `
    --idl "$wchar_in_relative"

& "$base_dir\..\src\target\start.bat" `
    --cpp-out "$temp_out_relative/cpp" `
    --cpp-namespace testsuite `
    --ident-cpp-enum-type foo_bar `
    --cpp-optional-template "std::experimental::optional" `
    --cpp-optional-header "\`"../../handwritten-src/cpp/optional.hpp\`"" `
    --cpp-extended-record-include-prefix "../../handwritten-src/cpp/" `
    `
    --cs-namespace Djinni.TestSuite `
    --cppcli-out "$temp_out_relative/cppcli" `
    `
    --yaml-out "$temp_out_relative/yaml" `
    --yaml-out-file "yaml-test.yaml" `
    --yaml-prefix "test_" `
    `
    --idl "$in_relative" `
    --idl-include-path "djinni/vendor"

# Make sure we can parse back our own generated YAML file
cp "$base_dir/djinni/yaml-test.djinni" "$temp_out/yaml"
& "$base_dir\..\src\target\start.bat" `
    --cpp-out "$temp_out_relative/cpp" `
    --ident-cpp-enum-type foo_bar `
    --cpp-optional-template "std::experimental::optional" `
    --cpp-optional-header "\`"../../handwritten-src/cpp/optional.hpp\`"" `
    `
    --cs-namespace Djinni.TestSuite `
    --cppcli-out "$temp_out_relative/cppcli" `
    `
    --idl "$temp_out_relative/yaml/yaml-test.djinni"
