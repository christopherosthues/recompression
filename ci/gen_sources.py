import os
import os.path

for root, sub_dirs, files in os.walk("."):
    for file in files:
        if file.endswith(".hpp"):
            r = root[1:] if root == "." else root[2:]
            os.makedirs("../src/" + r + "/", exist_ok=True)
            source_file = open("../src/" + r + "/" + file[:len(file)-3] + "cpp", 'w')
            source_file.write('#include "' + file + '"')
            source_file.close()
