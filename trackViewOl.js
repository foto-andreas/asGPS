// style the vectorlayer
var styleMap = new OpenLayers.StyleMap({
    'default': new OpenLayers.Style({
        strokeColor: "yellow",
        strokeWidth: 1.2,
        strokeOpacity: 1
    })
});

var track = new OpenLayers.Layer.Vector("Tracks", {styleMap: styleMap});

var trackPoints = new OpenLayers.Layer.Vector("Track points");
var wayPoints =  new OpenLayers.Layer.Vector("Way points");
var routePoints =  new OpenLayers.Layer.Vector("Route points");

function trackViewInit() {
    map.addLayers([track, trackPoints, wayPoints, routePoints]);
}

function trackView() {

    track.removeAllFeatures();
    trackPoints.removeAllFeatures();
    wayPoints.removeAllFeatures();
    routePoints.removeAllFeatures();

    var nodes = [];

    for (i=0; i<api.getTrackSize(); i++) {
        var loc = api.getTrackPoint(i).split(':',4);
        var l = new OpenLayers.LonLat(loc[1], loc[0]).transform(defPro, map.getProjectionObject());
        if (loc[2] == "0") { // track
            nodes.push(new OpenLayers.Geometry.Point(l.lon, l.lat));
//            trackPoints.addFeatures([new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Point(l.lon, l.lat), null, style_red)]);
//            debug("track point " + loc);
        }
        if (loc[2] == "1") { // way
//            wayPoints.addFeatures([new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Point(l.lon, l.lat), null, style_red)]);
//            debug("way point " + loc);
        }
        if (loc[2] == "2") { // route
//            routePoints.addFeatures([new OpenLayers.Feature.Vector(new OpenLayers.Geometry.Point(l.lon, l.lat), null, style_red)]);
//            debug("route point " + loc);
        }
    };

    track.addFeatures([new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LineString(nodes).simplify(2))]);

    debug("Extent: " + track.getDataExtent());
    map.zoomToExtent(track.getDataExtent());

}
