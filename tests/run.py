import os
import sys
from typing import List

if __name__ == "__main__":
    pathBase = './tests/myLab/Tests_2_Advanced'
    paramList: List[str] = sys.argv
    if len(paramList) > 1:
        fileList = [f'{i[i.rfind("/")+1:].replace(".cmm", "")}.cmm' for i in paramList[1:]]
    else:
        fileList = [i for i in os.listdir(pathBase) if i.endswith('.cmm')]
    for i in fileList:
        print(i.replace('.cmm', ''))
        if os.path.isfile(f'{pathBase}/{i}'):
            os.system(f'./parser {pathBase}/{i} 2> {pathBase}/{i.replace(".cmm", ".out.txt")}')
        else:
            os.system(f'./parser {pathBase}/{i}')