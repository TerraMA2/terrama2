# Beta 2

## New Features:
- Alert and notification service
- Conditional schedule
- Icons in the Webapp
- Pre-compiled packages for Ubuntu 14.04 & 16.04
- Generic driver for CSV files
  - Occurrence
  - DCP
- Email and PDF report as attachment

## Enhancements:
- Import DCP configuration file
- Better filter by value
  - Protection from sql injections
- Allow compilation of the service without all modules
  - Allow compiling modules without some libraries (VMime, Qt < 5.8)
- Update Terralib to 5.2.1
- Use of terralib CurlWrapper for FTP downloads
- New operators for analysis:
  - grid zonal forecast num and list
  - grid zonal forecast interval num and list
- New options for FTP:
  - Timeout
  - Active mode
- Analises operators menus
- New service status: Interrupted


## bugfix:
- Fix service error messages
  - Better messages
  - Don't give out of context errors
- Fix project export/import
- Fix Uri tags for map and email servers
- Using timezone from dataset in DataRetriever downloads
