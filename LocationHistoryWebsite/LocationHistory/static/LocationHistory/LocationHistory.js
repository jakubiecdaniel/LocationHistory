
const log_list = JSON.parse(JSON.parse(document.getElementById('log_list_json').textContent));

let i = 0
for (const log of log_list) {

    i+=1
    var map = L.map('map'+i).setView([parseFloat(log.fields.location_latitude), parseFloat(log.fields.location_longitude)], 13);

    L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {
        attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
    }).addTo(map);
    
    var circle = L.circle([parseFloat(log.fields.location_latitude), parseFloat(log.fields.location_longitude)], {
      color: 'red',
      fillColor: '#f03',
      fillOpacity: 0.5,
      radius: parseFloat(log.fields.location_accuracy)
    }).addTo(map);
    
    
    var marker = L.marker([parseFloat(log.fields.location_latitude), parseFloat(log.fields.location_longitude)]).addTo(map);


}



