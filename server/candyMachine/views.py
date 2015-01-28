"""
" views.py
" --------
"
" Methods defined for creating the views of the demo
"""

"""
" To use Braintree it is mandatory to configure the API credentials, that could
" be gotten from Braintree account. In this example, we'll use Sandbox API
" credentials from the Commerce Factory Braintree account. More information at:
"    * github.com/commercefactory
"    * commercefactory.org
"""
import braintree
import tweetpony
import time

"""
" Importing library to communicate with Spark core
"""
from spyrk import SparkCloud

"""
" Using django shorcuts for rendering the html templates
"""

from django.shortcuts import render_to_response

"""
"-------------------------------------------
" 1. API credentials for Braintree v.zero
"""
BT_MERCHANT_ID = "<your merchant ID>"
BT_PUBLIC_KEY = "<your public key>"
BT_PRIVATE_KEY = "<your private key>"

braintree.Configuration.configure(
                braintree.Environment.Sandbox,
                merchant_id=BT_MERCHANT_ID,
                public_key=BT_PUBLIC_KEY,
                private_key=BT_PRIVATE_KEY
                )

"""
" 2. API credentials for Spark Core
"""
SP_USERNAME = "<Spark User name>"
SP_PASSWORD = "<Spark password>"
SP_ACCESS_TOKEN = "<Spark Access Token>"

"""
" 3. API credentials for TweetPony
"""
TP_CONSUMER_KEY = "<TweetPony consumer key>"
TP_CONSUMER_SECRET = "<TweetPony consumer secret>"
TP_ACCESS_TOKEN = "<TweetPony access token>"
TP_ACCESS_TOKEN_SECRET = "<TweetPony access token sercret>"

"""
"-------------------------------------------
"""

"""
" Method called at root path.
"""

def index(request):

    dateTime=time.strftime("%d/%m/%Y")
    return render_to_response("index.html",locals())

"""
" Method that gets the client_token and renders with it the payment.html page,
" showing the v.zero form.
"""

def payment(request):
    client_token=braintree.ClientToken.generate({})

    return render_to_response("payment.html",locals())

"""
" Method that processes the information gotten from payment form and finishes
" the transaction to Braintree. If Twitter alias is defined, it also calls to
" Twitter API to send the tweet.
" After doing it, calls to spark core, sending the transaction_id and tweet alias (if exists)
"""

def checkout(request):

    spark = SparkCloud(SP_USERNAME,SP_PASSWORD)

    """
    " Alternative:
    "spark = SparkCloud(SP_ACCESS_TOKEN)
    """

    if (spark.<YOUR_CORE_NAME>.connected):

        nonce=request.POST.get("payment_method_nonce")

        """
        " Finishing the transaction adn getting the result.
        """

        result=braintree.Transaction.sale({
            "amount":"1.00",
            "payment_method_nonce":nonce
        })

        transaction_id=result.transaction.id

        name = request.POST.get("twitterName")
        if name != "":

            """
            " Calling to Spark Core to show the tweet message at the tft
            """
            spark.<YOUR_CORE_NAME>.twitter(name)

            api = tweetpony.API(
                    consumer_key = TP_CONSUMER_KEY,
                    consumer_secret = TP_CONSUMER_SECRET,
                    access_token = TP_ACCESS_TOKEN,
                    access_token_secret = TP_ACCESS_TOKEN_SECRET
                    )
            try:
                text = "Hi @"+name+". This is your transaction ID:"+transaction_id+". Please, grab your candies!"
                api.update_status(text)
            except tweetpony.APIError as err:
                print "Oops, something went wrong! Twitter returned error #%i and said: %s" % (err.code, err.description)
            else:
                print "Yay! Your tweet has been sent!"

        """
        " Calling to Spark Core to move the motor

        """
        spark.<YOUR_CORE_NAME>.candy(transaction_id)

    else:

        textMessage = "ERROR: Device not connected"


    return render_to_response("checkout.html",locals())
