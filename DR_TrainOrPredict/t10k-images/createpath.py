import os, sys
def get_filepaths(directory):
    file_paths = []  # List which will store all of the full filepaths.
    for root, directories, files in os.walk(directory):
        for filename in files:
            # Join the two strings in order to form the full filepath.
            filepath = os.path.join(root, filename)
            file_paths.append(filepath)  # Add it to the list.
    return file_paths  # Self-explanatory.
lists = get_filepaths(os.path.dirname(os.path.abspath(__file__)))
with open('result.txt', 'w') as f:
    for url in lists:        
        if (os.path.basename(url).startswith('0_')):
            f.write(url)
            f.write('\n')
            f.write('0\n')
        if (os.path.basename(url).startswith('1_')):
            f.write(url)
            f.write('\n')
            f.write('1\n')
        if (os.path.basename(url).startswith('2_')):
            f.write(url)
            f.write('\n')
            f.write('2\n')
        if (os.path.basename(url).startswith('3_')):
            f.write(url)
            f.write('\n')
            f.write('3\n')
        if (os.path.basename(url).startswith('4_')):
            f.write(url)
            f.write('\n')
            f.write('4\n')
        if (os.path.basename(url).startswith('5_')):
            f.write(url)
            f.write('\n')
            f.write('5\n')
        if (os.path.basename(url).startswith('6_')):
            f.write(url)
            f.write('\n')
            f.write('6\n')
        if (os.path.basename(url).startswith('7_')):
            f.write(url)
            f.write('\n')
            f.write('7\n')
        if (os.path.basename(url).startswith('8_')):
            f.write(url)
            f.write('\n')
            f.write('8\n')
        if (os.path.basename(url).startswith('9_')):
            f.write(url)
            f.write('\n')
            f.write('9\n')
