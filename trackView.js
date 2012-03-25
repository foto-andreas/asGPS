function loadXMLString(txt) {
  parser=new DOMParser();
  xmlDoc=parser.parseFromString(txt, "text/xml");
  return xmlDoc;
}

var poly = new google.maps.Polyline();

function trackView() {

    poly.setMap(null);

    var xml = api.getTrackData();

    var gps = loadXMLString(xml);

    var p = gps.getElementsByTagName("trkpt");

    var points = [];

    for (i=0; i<p.length; i++) {
        var lat = ((p[i].attributes).getNamedItem("lat")).nodeValue;
        var lon = ((p[i].attributes).getNamedItem("lon")).nodeValue;
        var l = new google.maps.LatLng(lat, lon);
        points.push(l);
    };

    poly = new google.maps.Polyline({
          path: points,
          strokeColor: "#FF00AA",
          strokeOpacity: .4,
          strokeWeight: 2
        });

    poly.setMap(map);

}
