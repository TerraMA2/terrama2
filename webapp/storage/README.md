# Storage Algorithm

The storage is applied on dynamic data and analysis results.
This data can be tiff files or tables in a database. In case of tables can be multiple tables or a single table. In this way the algorithm has 3 main functions:

## StoreTIFF
Executes storage over tiff files.

- Checks in the control chart the date of the last collection or successful analysis, from that date it is calculated to which date the files will be erased as defined by the user
- The date of the tiffs images is in the file, so it is necessary to use the Regex to set the date of these
- Delete or move the files that satisfy the date request
- If the user asks to reset the filter, the history present in the control table is moved to the storage history file. Thus the new collection/analysis is executed as if it had never been executed

## StoreSingleTable and StoreNTable
Executes storage over tables in the database. The difference between the single and multiple tables is that in the first case the table name and the date attribute are fixed by the TerraMA2 and in the second case they are defined by the user.

- Check in the control chart the date of the last collection or successful analysis, from that date it is calculated to which date the records will be erased as defined by the user
- The date of the records are defined by a column in the table itself
- Clears or moves records that satisfy the date request
- If the user asks to reset the filter, the history present in the control table is moved to the storage history file. Thus the new collection/analysis is executed as if it had never been executed
