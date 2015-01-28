from django.conf.urls import patterns, include, url
from django.conf.urls.static import static

urlpatterns = patterns('',

    url(r'^$', 'candyMachine.views.index', name='index'),
    url(r'^index', 'candyMachine.views.index', name='index'),
    url(r'^payment', 'candyMachine.views.payment', name='payment'),
    url(r'^checkout', 'candyMachine.views.checkout', name='checkout'),
)
