# bar
Portable incremental backup utility written in C without any dependencies.

# Usage
**Create first backup of folder**

`bar b <input folder path> <output folder path>`

This will copy all files from input to output folder and create a file ".bar" in output folder that holds checksums for all stored files.

Each time the exact same command as above will be executed after the first backup the application will look for changes in input folder and modify the output folder content to match the input folder content by just deleting and copying the necessary files and subdirectories (incremental).

This is done by comparing the checksums for the input folder created during each run with the checksums stored in ".bar" data file to be found in backup (output) folder.

**Verify ".bar" file in backup (output) folder**

`bar v <output folder path>`

Checks, if the ".bar" file in the backup (output) folder matches the actual content of the backup folder.

**(Re-)create ".bar" file in backup (output) folder**

`bar c <output folder path>`

(Re-)creates the ".bar" data file. File's content will reflect the backup (output) folder's contents.

# License
See the [LICENSE](https://github.com/RhinoDevel/bar/blob/master/LICENSE.md) file for license rights and limitations (ISC).
