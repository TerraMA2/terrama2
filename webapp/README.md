# TerraMA² Webapp

**NOTE:**
* **Until we reach version 4.0.0 this codebase will be instable and not fully operational.**
* **TerraMA² Webapp is under active development. We are preparing this site to host it!**
* **If you want to try the old version, please, look at http://www.dpi.inpe.br/terrama2.**
* **If you have any question, please, send us an e-mail at: terrama2-team@dpi.inpe.br.**

TerraMA² Webapp is a free and open source component based web API for use in web geographic information systems (WebGIS).

## Dependencies

In order to use the components of the API, you have to run it. For that to be possible you'll need to install some third-party softwares. Bellow we show you this softwares and its versions:

- **Node.js (Mandatory):** To run the API you need to install the Node.js interpreter. Make sure to have at least version 4.2.3 installed. You can download it from: https://nodejs.org/.

## Execution Instructions

Bellow we show the steps to run TerraMA² Webapp:

- Clone the TerraMA² repository:

```
git clone https://github.com/TerraMA2/terrama2.git
```

Note: This repository contain the entire TerraMA², if you want just the web API, the important is the webapp directory.

- Access the webapp directory via command line and run the following command:

```
npm start
```

- By default the API will run in the port 36000, you can change this in the file terrama2/webapp/bin/www:

```
var portNumber = '36000';
```
