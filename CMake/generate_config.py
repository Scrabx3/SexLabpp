import os
import re

config_def_path = './src/UserData/config.def'
output_ini_path = './CMake/gen/SexLab.ini'

def parse_config_def(file_path):
  settings = []
  with open(file_path, 'r') as file:
    for line in file:
      match = re.match(r'INI_SETTING\(([^,]+),\s*([^,]+),\s*"([^"]+)"\)', line)
      if match:
        value, default, category = match.groups()
        settings.append((value.strip(), default.strip(), category.strip()))
  return settings

def generate_ini_file(settings, output_path):
  categories = {}
  for value, default, category in settings:
    if category not in categories:
      categories[category] = []
    categories[category].append((value, default))
  
  with open(output_path, 'w') as file:
    for category, values in categories.items():
      file.write(f'[{category}]\n')
      for value, default in values:
        file.write(f'{value} = {default}\n')
      file.write('\n')

settings = parse_config_def(config_def_path)
generate_ini_file(settings, output_ini_path)

print(f"Generated {output_ini_path}")