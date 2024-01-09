#open Test/NMEAFile.txt and replace inplace '\n' with '\r\n'

import os
import sys


filename = "Test/NMEAFile.txt"
outputFilename = "Test/NMEAFileCorrected.txt"
with open(filename, 'r') as f:
    content = f.read()
content = content.replace('\n', '\r\n')
with open(outputFilename, 'w') as f:
    f.write(content)