## Logger

TerraMA² webapp uses NodeJS winston library for logging.
In order to format the output, we are using functions ```formatLogArguments``` and ```getStackInfo``` provided by fisch0920 that retrieve the filename and line number from stack trace. ([Link](https://gist.github.com/fisch0920/39a7edc77c422cbf8a18))