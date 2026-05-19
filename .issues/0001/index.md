Title: Segfault when using `file_read` on directories
Author: am00ma
Created: Sun, 17 May 2026 14:43:45 +0200
State: closed

Initially was checking `file_exists` which is true for dirs as well.

Corrected by using stat info instead.
