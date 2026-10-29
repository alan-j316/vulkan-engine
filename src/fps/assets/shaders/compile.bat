echo Compiling Shaders...
%VULKAN_SDK%/Bin/slangc.exe Simple.frag.slang -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry fragMain -o Simple.frag.spv
%VULKAN_SDK%/Bin/slangc.exe Simple.vert.slang -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -o Simple.vert.spv

echo Shader Compilation Complete!