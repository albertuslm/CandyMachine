Candy Machine
============

IoT project, using a common Candy dispenser machine, which has been previously hacked, a Spark core (www.spark.io) and Braintree v.zero SDK.

![example](https://github.com/albertuslm/CandyMachine/blob/master/media/repository/image1.JPG)

## Technology
Here you have a teaser video with the full demo, enjoy!

[TO YOUTUBE](https://www.youtube.com/watch?v=n6iCVJZaXE4)

> Working on a 2nd video demo.

## Technology

**Hardware used:**

* [Candy Machine](http://www.ebay.es/itm/271330987882)
* [Spark Core](https://www.spark.io/)
* [2,2" TFT with SD](http://goo.gl/lDS3FH)
* [CC motor 5V](http://goo.gl/zLjKL4)
* generic SD 1GB.

**Server:**

* [Python](https://www.python.org/): V.2.7.6.
* [Django](https://www.djangoproject.com/): Python web framework. V. 1.7.1.
* The [Braintree Python SDK](https://github.com/braintree/braintree_python)
* [TweetPony](https://pypi.python.org/pypi/TweetPony)
* [spyrk API](https://pypi.python.org/pypi/spyrk)

**Spark Core:**

* [ILI9341 for Spark](https://github.com/pkourany/ILI9341_SPI_LCD)
* [SD library for Spark](https://github.com/technobly/SparkCore-SD)

## Running the server

* In the `server` folder
* Run `pip install braintree` to install Braintree
* Run `pip install spyrk` to install spyrk
* Run `pip install tweetpony` to install tweetpony
* Run `python manage.py runserver` to start the server

## Running the application at Spark Core

> It is strongly recommended to use [Spark Dev IDE](https://www.spark.io/dev)

* Deploy the "spark" folder on your local system.
* Add the following Credentials in order to use them:
  * Braintree sandbox credentials
  * Twitter Dev Credentials
  * Spark Credentials
* Change the string <yourDeviceCode> to your Spark core name.
* Compile and upload the firmware to the core.

## SD Configuration

* Copy the following images to the root path of your SD Card:
  * btdev.bmp
  * twitter.bmp
  * motor.bmp

> You can find them here: [SDCardImages](https://github.com/albertuslm/CandyMachine/tree/master/media/SD_images)

## Wiring the core

> under construction

## Testing the demo
* Open on a web browser the url "127.0.0.1:8000"
* Click on `Start Payment`
* Select either of these payment methods:
* (PayPal)
* Fill in the following credentials:
* Email: `us-customer@commercefactory.org`
* Password: `test1234`
* Click `Agree` to accept future payments
* (Credit Card)
* Fill in the following credentials:
* Number: `4111 1111 1111 1111`
* Expiration date: `11/15`
* CVV: `123`
* Click on `Pay`
* You will see a message that says __"Subscription ID: [subscription_id]"__

> NOTE: If you add your twitter alias in the form, you'll receive a tweet from your spectacular Candy Machine's Twitter account.


## Useful links

* [Full documentation for the Braintree Client SDK for iOS+Python](https://developers.braintreepayments.com/ios+python/start/overview)
