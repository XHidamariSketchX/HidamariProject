from django.conf.urls import patterns,url
from con_to_weibo import views
urlpatterns = patterns('',
	url(r'^$',views.redirect_to_con,name="con_to_view"),
	url(r'^call_back/$',views.call_back,name="call_back"),

)
