# TerraMA² Webapp

**NOTE:**
* **Until we reach version 4.0.0 this codebase will be instable and not fully operational.**
* **TerraMA² Webapp is under active development. We are preparing this site to host it!**
* **If you want to try the old version, please, look at http://www.dpi.inpe.br/terrama2.**
* **If you have any question, please, send us an e-mail at: terrama2-team@dpi.inpe.br.**

TerraMA² Webapp is a free and open source component based web API for use in web geographic information systems (WebGIS).

## Dependencies

In order to use the components of the API, you have to run it. For that to be possible you'll need to install some third-party softwares. It can be found in TerraMA² repository [WEB DEPENDENCIES](https://github.com/TerraMA2/terrama2#web-application-and-components-dependencies) section.

## Installation

Bellow we show the steps to run TerraMA² Webapp:

- Clone the TerraMA² repository:

```
git clone https://github.com/TerraMA2/terrama2.git
```

Note: This repository contain the entire TerraMA², if you want just the web API, the important is the webapp directory.

- Access the webapp folder and run the following nodejs command:

```bash
cd webapp
sudo npm install -g bower # It install bower in system path
sudo npm install -g grunt-cli # It install grunt in system path
npm install # It installs backend web dependencies
bower install # It installs front end dependencies
grunt # To compile and prepares TerraMA2 FrontEnd app
```

## Execution Instructions

TerraMA2 web application uses NodeJS Sequelize ORM for handling database manipulation. Make sure you have PostgreSQL database and PostGIS spatial extension installed before.

In order to execute, check available contexts names in [config/config.json](https://raw.githubusercontent.com/TerraMA2/terrama2/master/webapp/config/config.terrama2) (Feel free to add a new one following section [Configuration File](#configuration-file)).

After that, run ```npm start ContextName``` where **ContextName** is a key pointing to context credentials. 
Example:

```bash
npm start # it will use default credentials
npm start tests # it will use credentials of tests context
```

Remember that context name is optional. By default, TerraMA² will use ```default``` credentials;

## Configuration File
The TerraMA² Web settings can be found in [config/config.json](https://raw.githubusercontent.com/TerraMA2/terrama2/master/webapp/config/config.terrama2).

Key               | Description
----------------- | -----------------------------------------------------------------
 port             | A TerraMA² Application Port Number 
 db               | Database credentials 
 db.username      | username
 db.password      | password 
 db.host          | host of database 
 db.port          | database port number 
 db.dialect       | dialect database (postgres/mysql). **Only postgres supported.** 
 db.define        | It represents a extra properties along database 
 db.define.schema | A database schema 