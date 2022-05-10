from django.http.response import HttpResponseNotFound, HttpResponse, HttpResponseForbidden, HttpResponseBadRequest
from django.shortcuts import render
from django.shortcuts import redirect
from django.views.decorators.csrf import csrf_exempt
from django import forms
from .models import LocationHistory
from .models import LocationHistoryUser
from django.core.paginator import Paginator
from django.core import serializers
from django.core.exceptions import ValidationError
import json
import datetime
# Create your views here.

record_per_page_chocies = [('1','1')] + [(str(x),str(x)) for x in range(5,25,5)]
order_by_choices = [('',"Select"),("lat","Latitude"),("long","Longitude")]

def generateUUID(request):

    uuid = LocationHistoryUser()

    uuid.save()

    return HttpResponse(uuid.user_id)

def index(request):
    if request.get_full_path() == '/':
        return redirect('/LocationHistory/')
    return render(request, 'LocationHistory/LocationHistoryIndex.html')

def view_location_single(request,user_id,log_id):
    
    location = LocationHistory.objects.get(user=LocationHistoryUser.objects.get(pk=user_id),log_id=log_id)

    return render(request, 'LocationHistory/LocationHistorySingle.html', {'location' : location})

class LocationFilterForm(forms.Form):
    from_date = forms.DateField(label="From Date",required=False,widget=forms.DateInput(attrs={'type': 'date','name':'from-date'}))
    to_date = forms.DateField( label="To Date",required=False,widget=forms.DateInput(attrs={'type': 'date','name':'to-date'}))

    to_date.widget.attrs.update({'class':'form-control'})
    from_date.widget.attrs.update({'class':'form-control'})

    order_by = forms.ChoiceField(label="Order By",choices=order_by_choices,required=False,initial='')
    order_by.widget.attrs.update({'class':'form-select'})
    
    records_per_page = forms.ChoiceField(label="Records Per page",choices=record_per_page_chocies,required=False,initial='10')
    records_per_page.widget.attrs.update({'class':'form-select'})

def view_location_list(request,user_id):
    
    from_date = request.GET.get('from_date', None)
    to_date = request.GET.get('to_date',None)
    order_by = request.GET.get('order_by',None)
    records_per_page = request.GET.get('records_per_page')

    try:
        user = LocationHistoryUser.objects.get(pk=user_id)
    except LocationHistoryUser.DoesNotExist:
        return HttpResponseNotFound("User Not Found") #Todo, redirect home and display this message?

    log_list = user.LocationHistoryLogs.all()

    form = LocationFilterForm()

    if from_date != '' and from_date != None:
        form.fields['from_date'].initial = from_date
        log_list = log_list.filter(location_datetime__gte=from_date)
    if to_date != '' and to_date != None:
        form.fields['to_date'].initial = to_date
        log_list = log_list.filter(location_datetime__lte=to_date)
    if order_by != '':
        form.fields['order_by'].initial = order_by
        if order_by == 'lat':
            log_list = log_list.order_by('location_latitude')
        elif order_by == 'long':
             log_list = log_list.order_by('location_longitude')
        elif order_by == 'both':
            pass
    if records_per_page:
        form.fields['records_per_page'].initial = records_per_page
        p = Paginator(log_list,records_per_page)
    else:
        p = Paginator(log_list,5)

    page = request.GET.get('page')
    if page == None:
        page = '1'
    
    list = p.get_page(page)

    page_list = []
    if page:
        page_num = int(page)

        if 1 < page_num - 5:
            start = page_num - 5
        else:
            start = 1
        
        if list.paginator.num_pages > page_num + 5:
            end = page_num + 5
        else:
            end = list.paginator.num_pages

        for i in range(start,end + 1):
            page_list += [str(i)]

    
    log_list_json = serializers.serialize("json",list)

    return render(request, 'LocationHistory/LocationHistoryList.html', {'log_list':list,'log_list_json':log_list_json,'form':form,'page_list':page_list})

@csrf_exempt
def new_location(request):
    
    if request.method == "POST":
        print(request.body)
        
        try:
            data = json.loads(request.body)
            location_data = data['location']['location']
            
            lat = location_data['lat']
            long = location_data['lng']
            accuracy = data['location']['accuracy']
            
            user_id = data['user']
        except KeyError as err:
             return HttpResponseBadRequest("Key Error: " + str(err))

        try:
            x = LocationHistoryUser.objects.get(pk=user_id)
        except ValidationError as err:
            return HttpResponseBadRequest(err)
        else:
            new_log = LocationHistory(location_latitude=lat, location_longitude=long, location_accuracy=accuracy,location_datetime=datetime.datetime.now(),user=x)
            new_log.save()

        return HttpResponse(f"Log Created: \nLat: {lat}\nLong: {long}\nAccuracy: {accuracy}")
        

    else:
        return HttpResponseForbidden('Must be POST')