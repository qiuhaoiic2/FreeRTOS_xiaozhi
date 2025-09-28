import os
import json
import glob

def find_include_dirs(root_dir):
    """查找所有包含头文件的目录"""
    include_dirs = set()

    # 搜索所有的 .h 文件
    for h_file in glob.glob(os.path.join(root_dir, "**/*.h"), recursive=True):
        include_dir = os.path.dirname(h_file)
        # 相对于项目根目录的路径
        rel_path = os.path.relpath(include_dir, root_dir)
        # 转换为 VSCode 格式的路径 (使用正斜杠)
        include_path = "${workspaceFolder}/" + rel_path.replace("\\", "/")
        include_dirs.add(include_path)

    # 特殊情况：也添加根目录
    include_dirs.add("${workspaceFolder}")

    return sorted(list(include_dirs))

def update_cpp_properties(root_dir, include_dirs):
    """更新 c_cpp_properties.json 文件"""
    cpp_props_path = os.path.join(root_dir, ".vscode", "c_cpp_properties.json")

    # STM32F1默认配置
    stm32f1_config = {
        "cStandard": "c99",
        "compilerPath": "F:/gcc-arm-none-eabi-10.3-2021.10-win32/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc",
        "cppStandard": "c++11",
        "defines": [
            "USE_HAL_DRIVER",
            "STM32F103xE"
        ],
        "intelliSenseMode": "windows-gcc-arm",
        "name": "stm32f1",
        "includePath": include_dirs,
        "browse": {
            "limitSymbolsToIncludedHeaders": True
        }
    }

    try:
        # 检查文件是否存在
        if os.path.exists(cpp_props_path):
            with open(cpp_props_path, 'r', encoding='utf-8') as f:
                cpp_props = json.load(f)

            # 更新配置
            config_found = False
            for i, config in enumerate(cpp_props["configurations"]):
                if config["name"] == "stm32f1":
                    # 保留原有的includePath
                    stm32f1_config["includePath"] = include_dirs
                    cpp_props["configurations"][i] = stm32f1_config
                    config_found = True
                    break

            if not config_found:
                cpp_props["configurations"].append(stm32f1_config)
        else:
            # 如果文件不存在，创建新的
            os.makedirs(os.path.dirname(cpp_props_path), exist_ok=True)
            cpp_props = {
                "configurations": [stm32f1_config],
                "version": 4
            }

        # 写回文件
        with open(cpp_props_path, 'w', encoding='utf-8') as f:
            json.dump(cpp_props, f, indent=4)

        print(f"成功更新 {cpp_props_path}")
        print(f"添加了 {len(include_dirs)} 个包含路径")

    except Exception as e:
        print(f"更新文件时出错: {str(e)}")

if __name__ == "__main__":
    # 获取当前工作目录（假设是项目根目录）
    root_dir = os.path.abspath(os.path.dirname(__file__))

    print(f"正在搜索目录: {root_dir}")
    include_dirs = find_include_dirs(root_dir)

    print(f"找到 {len(include_dirs)} 个包含目录")
    update_cpp_properties(root_dir, include_dirs)

    # 打印所有找到的路径
    print("\n所有包含路径:")
    for path in include_dirs:
        print(f"  {path}")
