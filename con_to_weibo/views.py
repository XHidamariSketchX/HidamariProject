#!/usr/bin/python
#-*-coding:utf-8-*-
from django.shortcuts import render
from django.http import HttpResponse, HttpResponseRedirect
from weibo import APIClient
APP_KEY = '2732451581'
APP_SECRET = '4039d13c60fd03bdea77aeaabd304e09'
CALLBACK_URL = 'http://127.0.0.1:8000/con_to_weibo/call_back'
def redirect_to_con(request):
	client = APIClient(app_key=APP_KEY, app_secret=APP_SECRET,redirect_uri=CALLBACK_URL)
	url = client.get_authorize_url()
	return HttpResponseRedirect(url)
#	return HttpResponse('you willl be lead to Auth2.0')
def call_back(request):
	
	code = request.GET.get('code')
	client = APIClient(app_key=APP_KEY, app_secret=APP_SECRET,redirect_uri=CALLBACK_URL)
	r= client.request_access_token(code)
	_access_token = r.access_token
	expires_in = r.expires_in
	client.set_access_token(_access_token, expires_in)
#	client.statuses.update.post(access_token=_access_token,status=u'My first po from HidamariProject')
	client.statuses.update.post(access_token=_access_token,status=u'Hello @Rokyubu @Moen大小姐 @推倒阿卡林的_zsh123 @阿knell的可可二加一 from Python|Django|HidamariProject')

	r = client.statuses.user_timeline.get(access_token=_access_token,uid=3205570100)
	ret = ''
	for st in r.statuses:
		ret=ret + st.text
#	print client.statues.update.post(status=u'test HidamariProject')
	
	return HttpResponse(ret)
 	
	
# Create your views here.
