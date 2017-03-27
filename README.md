# pib
**P**ortable **i**ncremental **b**ackup utility written in C (C11) without any dependencies.

# Usage
**Create backup of folder (usually the only command you need)**

`pib b <input folder path> <output folder path>`

This snipit will copy all the files from the input to the output folder and create a file `.pib` in the output folder that holds checksums for all stored files.

Each time the exact same command as above is executed, after the first backup, the application will look for changes in the input folder and modify the output folder contents to match the input folder's contents by just deleting, creating or updating the necessary files and subdirectories (incremental).

This is done by comparing the file checksums and file sizes for the input folder created during each run with the checksums and sizes stored in `.pib` data file found in backup (output) folder.

The deletions, creations and updates planned will be presented to the user beforehand.

If deletions and/or updates of files are necessary, the user will be asked, if the application shall proceed.

The `.pib` data file will be updated during the backup process (to match the new content of output folder).

**Verify data file in backup (output) folder**

`pib v <output folder path>`

Checks, if the `.pib` file in the backup (output) folder matches the actual content of the backup folder.

**(Re-)create data file in backup (output) folder**

`pib c <output folder path>`

(Re-)creates the `.pib` data file. File's content will reflect the backup (output) folder's contents.

**Show folder informations**

`pib i <folder path>`

Prints count and (byte) size of all files and subfolders included in folder at given path.

# License
See the [LICENSE](https://github.com/RhinoDevel/pib/blob/master/LICENSE.md) file for license rights and limitations (ISC).
