import os
import os.path

for root, sub_dirs, files in os.walk("."):
    for file in files:
        if file.endswith(".hpp"):
            r = root[1:] if root == "." else root[2:]
            os.makedirs("../src/" + r + "/", exist_ok=True)
            source_file = open("../src/" + r + "/" + file[:len(file)-3] + "cpp", 'w')
            if root[2:] == "":
                source_file.write('#include "' + file + '"\n')
            else:
                source_file.write('#include "' + root[2:] + "/" + file + '"\n')
            source_file.close()
