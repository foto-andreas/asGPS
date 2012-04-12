var polyTrack = new google.maps.Polyline();

var markers = [];

function trackView() {

    polyTrack.setMap(null);
    delete polyTrack;
    polyTrack = new google.maps.Polyline();

    markers.foreach( function (k, m) {
        m.setMap(null);
        delete m;
    });
    markers = [];

    track = [];

    for (i=0; i<api.getTrackSize(); i++) {
        loc = api.getTrackPoint(i).split(':',4);
        l = new google.maps.LatLng(loc[0], loc[1]);
        if (loc[2] == "0") { // track
            track.push(l);
        }
        if (loc[2] == "1") { // way
            m = new google.maps.Marker({
                  position: l,
                  map: map,
                  icon: new google.maps.MarkerImage("http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|9090FF"),
                  title: loc[3]});
            markers.push(m);
        }
        if (loc[2] == "2") { // route
            m = new google.maps.Marker({
                  position: l,
                  map: map,
                  icon: new google.maps.MarkerImage("http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|90EE90"),
                  title: loc[3]});
            markers.push(m);
        }
    };

    polyTrack = new google.maps.Polyline({
          path: track,
          strokeColor: "#FF00AA",
          strokeOpacity: .4,
          strokeWeight: 2
        });


    polyTrack.setMap(map);

}
