from django.urls import path

from . import views

app_name = 'LocationHistory'
urlpatterns = [
    path('', views.index, name='index'),
    path('add/',views.new_location, name='new_location'),
    path('generateUUID',views.generateUUID,name='generateUUID'),
    path('<uuid:user_id>/',views.view_location_list,name='view_location_list'),
    path('<uuid:user_id>/<uuid:log_id>',views.view_location_single,name='view_location_single'),
]