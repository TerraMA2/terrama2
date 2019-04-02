# TerraMA² Webapp

**NOTE:** **Check the website for the most recent stable version: http://www.terrama2.dpi.inpe.br/**
* **If you have any question, please, send us an e-mail at: terrama2-team@dpi.inpe.br.**

TerraMA² Webapp is a free and open source component based web API for use in web geographic information systems (WebGIS).

## Installation

Before you start the installation, make sure you have installed Node.js version 8.12.0 LTS. You can download it from https://nodejs.org.

Bellow we show the steps to run TerraMA² Webapp:

- Clone the TerraMA² repository:

```
git clone https://github.com/TerraMA2/terrama2.git
```

Note: This repository contain the entire TerraMA², if you want just the web API, the important is the webapp directory.

- Access the webapp folder and run the following nodejs command:

```bash
cd webapp
sudo npm install -g grunt-cli # It install grunt in system path
npm install # It installs backend web dependencies
grunt # To compile and prepares TerraMA2 FrontEnd app
```

## Execution Instructions

TerraMA2 web application uses NodeJS Sequelize ORM for handling database manipulation. Make sure you have PostgreSQL database and PostGIS spatial extension installed before.

Copy the folder `config/sample_instances` to `config/instances` and follow the instructions to customize the default configuration below.

In order to execute, check available contexts names in [config/instances/config.json](https://raw.githubusercontent.com/TerraMA2/terrama2/master/webapp/config/webapp.json) (Feel free to add a new one following section [Configuration File](#configuration-file)).

After that, run ```npm start ContextName``` where **ContextName** is a key pointing to context credentials.
Example:

```bash
npm start # it will use default credentials
```

Remember that context name is optional. By default, TerraMA² will use ```default``` credentials;

## Configuration File
The TerraMA² Web settings can be found in [config/instances/config.json](https://raw.githubusercontent.com/TerraMA2/terrama2/master/webapp/config/webapp.json).

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


## Web Application components dependencies


If you want to build yourself TerraMA² then you need to install some third-party libraries. Below we show the list of third-party libraries dependencies and its versions:

- **Node.js (Mandatory):** Node.js is a JavaScript runtime for server side scripts. Make sure you have version 8.12.0 LTS installed. You can download it from: https://nodejs.org.

- **AdminLTE (Mandatory):** TerraMA² web front-end is built on top of AdminLTE web template. You can download it from: https://github.com/almasaeed2010/AdminLTE.

- **Bootstrap (Mandatory):** TerraMA² web front-end is built on top of Bootstrap. You can download it from: http://getbootstrap.com.

- **Chart.js (Mandatory):** Chart.js is an HTML5 based framework for drawing charts. You can download it from: http://www.chartjs.org.

- **DataTables (Mandatory):** DataTables is a table plugin for jQuery. It is available at: http://www.datatables.net.

- **jQuery (Mandatory):** jQuery is a JavaScript library for handling HTML document elements. Make sure to have at least version 2.1.4. It is available at: https://jquery.com.

- **AngularJS (Mandatory):** AngularJS is a SuperHeroic javascript MTW framework for handling client side application. Make sure to have at least version 1.6.0. It is available at: https://angularjs.org/.
