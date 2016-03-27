# pib
**P**ortable **i**ncremental **b**ackup utility written in C (C11) without any dependencies.

# Usage
**Create backup of folder (usually the only command you need)**

`pib b <input folder path> <output folder path>`

This will copy all files from input to output folder and create a file `.pib` in output folder that holds checksums for all stored files.

Each time the exact same command as above will be executed after the first backup the application will look for changes in input folder and modify the output folder content to match the input folder content by just deleting, creating or updating the necessary files and subdirectories (incremental).

This is done by comparing the checksums for the input folder created during each run with the checksums stored in `.pib` data file to be found in backup (output) folder.

The deletions, creations and updates planned will be presented to the user beforehand.

If deletions and/or updates of files are necessary, the user will be asked, if application shall really proceed.

The `.pib` data file will be updated during the backup process (to match the new content of output folder).

**Verify data file in backup (output) folder**

`pib v <output folder path>`

Checks, if the `.pib` file in the backup (output) folder matches the actual content of the backup folder.

**(Re-)create data file in backup (output) folder**

`pib c <output folder path>`

(Re-)creates the `.pib` data file. File's content will reflect the backup (output) folder's contents.

# License
See the [LICENSE](https://github.com/RhinoDevel/pib/blob/master/LICENSE.md) file for license rights and limitations (ISC).
