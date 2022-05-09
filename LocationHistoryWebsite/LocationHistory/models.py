from django.db import models
import uuid
import datetime
# Create your models here.

class LocationHistoryUser(models.Model):
    
    user_id = models.UUIDField(default=uuid.uuid4,primary_key=True,editable=True,unique=True)

class LocationHistory(models.Model):

    log_id = models.UUIDField(default=uuid.uuid4,editable=False,unique=True)

    location_latitude = models.CharField(max_length=20)

    location_longitude = models.CharField(max_length=20)

    location_accuracy = models.CharField(max_length=20)

    location_datetime = models.DateTimeField('date logged')

    user = models.ForeignKey(LocationHistoryUser, on_delete=models.CASCADE, related_name='LocationHistoryLogs')