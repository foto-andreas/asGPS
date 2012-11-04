
function debug(message) {
    console.log(message);
}

Array.prototype.foreach = function( callback ) {
    for( var k=0; k<this .length; k++ ) {
        callback( k, this[ k ] );
    }
}

String.prototype.trim = function () {
    return this.replace(/^\s*/, "").replace(/\s*$/, "");
}

var map, gmap, gphy, gsat, ghyb,
    defPro = new OpenLayers.Projection("EPSG:4326"),
    camPos, point, pointFeature, center;

// we want opaque external graphics and non-opaque internal graphics
var layer_style = OpenLayers.Util.extend({}, OpenLayers.Feature.Vector.style['default']);
layer_style.fillOpacity = 0.2;
layer_style.graphicOpacity = 1;

var style_orange = OpenLayers.Util.extend({}, layer_style);
style_orange.strokeColor = "#FF4F28";
style_orange.pointRadius = 10
style_orange.strokeWidth = 4;
style_orange.rotation = 45;
style_orange.strokeLinecap = "butt";
style_orange.graphicName = "cross";

var style_dark = OpenLayers.Util.extend({}, layer_style);
style_dark.strokeColor = "#D36F59";
style_dark.pointRadius = 10
style_dark.strokeWidth = 4;
style_dark.rotation = 45;
style_dark.strokeLinecap = "butt";
style_dark.graphicName = "cross";

var style_red = style_orange.clone();
style_red.strokeColor = "#900000";

function setMarker(lonlat) {
    camPos.removeAllFeatures();
    point = new OpenLayers.Geometry.Point(lonlat.lon,lonlat.lat);
    pointFeature = new OpenLayers.Feature.Vector(point, null, style_orange);
    camPos.addFeatures([pointFeature]);
}

function initialize() {

    map = new OpenLayers.Map('map_canvas',
                             {
                                 allOverlays: false,
                                 projection: new OpenLayers.Projection("EPSG:900913"),
                             });

    map.addControl(new OpenLayers.Control.LayerSwitcher());
    map.addControl(new OpenLayers.Control.TouchNavigation({
                                                              dragPanOptions: {
                                                                  enableKinetic: false
                                                              }
                                                          }));

    geocoder = new google.maps.Geocoder();

    elevator = new google.maps.ElevationService();

    var region = gup( "region" );

    var osm = new OpenLayers.Layer.OSM(
            "OpenStreetMap");

    var wms = new OpenLayers.Layer.WMS(
            "OpenLayers WMS",
            "http://vmap0.tiles.osgeo.org/wms/vmap0?", {layers: 'basic', visibility: false});

    gsat = new OpenLayers.Layer.Google(
                "Google Satellite",
                {type: google.maps.MapTypeId.SATELLITE, numZoomLevels: 22, visibility: false}
                );

    gphy = new OpenLayers.Layer.Google(
                "Google Physical",
                {type: google.maps.MapTypeId.TERRAIN, visibility: false}
                );

    gmap = new OpenLayers.Layer.Google(
                "Google Streets",
                {type: google.maps.MapTypeId.ROADMAP, numZoomLevels: 22, visibility: false}
                );

    ghyb = new OpenLayers.Layer.Google(
                "Google Hybrid",
                {type: google.maps.MapTypeId.HYBRID, numZoomLevels: 22, visibility: false}
                );

    map.addLayers([ghyb, gsat, gphy, gmap, osm, wms]);

    // Google.v3 uses EPSG:900913 as projection, so we have to
    // transform our coordinates
    debug("defPro = " + defPro + " / mapPro = " + map.getProjection());
    center = new OpenLayers.LonLat(0, 0).transform(defPro, map.getProjectionObject());
    debug("center = " + center);
    map.setCenter(center, 16);

    trafficLayer = new google.maps.TrafficLayer();
    bikeLayer = new google.maps.BicyclingLayer();
    weatherLayer = new google.maps.weather.WeatherLayer();
    cloudLayer = new google.maps.weather.CloudLayer();
    panoramioLayer = new google.maps.panoramio.PanoramioLayer();

    camPos = new OpenLayers.Layer.Vector("Camera position");

    map.addLayers([camPos]);

    setMarker(center);

    drag = new OpenLayers.Control.DragFeature(camPos, {
                                                  autoActivate: true,
                                                  onComplete: function(feature, pixel) {
                                                                  var lonlat = map.getLonLatFromPixel(pixel)
                                                                  debug("drag: " + lonlat);
                                                                  startElevator(lonlat);
                                                              }
                                              });

    map.addControl(drag);

    OpenLayers.Control.Click = OpenLayers.Class(OpenLayers.Control, {

                                                    defaultHandlerOptions: {
                                                        'single': true,
                                                        'double': false,
                                                        'pixelTolerance': 0,
                                                        'stopSingle': false,
                                                        'stopDouble': false
                                                    },

                                                    initialize: function(options) {
                                                                    this.handlerOptions = OpenLayers.Util.extend(
                                                                             {}, this.defaultHandlerOptions
                                                                             );
                                                                    OpenLayers.Control.prototype.initialize.apply(
                                                                                this, arguments
                                                                                );
                                                                    this.handler = new OpenLayers.Handler.Click(
                                                                             this, {
                                                                                 'click': this.onClick,
                                                                                 'dblclick': this.onDblclick
                                                                             }, this.handlerOptions
                                                                             );
                                                                },

                                                    onClick: function(evt) {
                                                                 var lonlat = map.getLonLatFromPixel(evt.xy);
                                                                 debug("click: " + lonlat);
                                                                 setMarker(lonlat);
                                                                 startElevator(lonlat);
                                                             },

                                                    onDblclick: function(evt) {
                                                                    debug("dblclick " + map.getLonLatFromPixel(evt.xy));
                                                                }

                                                });


    clicker = new OpenLayers.Control.Click();
    map.addControl(clicker);
    clicker.activate();

    map.addControl(new OpenLayers.Control.MousePosition({
                                                            displayProjection: new OpenLayers.Projection('EPSG:4326'),
                                                            numDigits: 5, separator: ' | lat = ', prefix: 'lon = '}));

    trackViewInit();

}

function startElevator(loc) {
    var defLoc = loc.clone().transform(map.getProjectionObject(), defPro);
    debug("startElevator: " + defLoc);
    locs = [];
    locs.push(new google.maps.LatLng(defLoc.lat, defLoc.lon));
    positionalRequest = {
        'locations': locs
    }
    elevator.getElevationForLocations(positionalRequest, function(results, status) {
                                          height = 0;
                                          if (status == google.maps.ElevationStatus.OK) {
                                              if (results[0]) {
                                                  height = results[0].elevation;
                                              } else {
                                                  debug("No elevator results found");
                                              }
                                          } else {
                                              debug("Elevation service failed due to: " + status);
                                          }
                                          api.marker_moved(defLoc.lat, defLoc.lon, height, toolsMap);
                                      });
}

function mapLayers(bike, traffic, weather, clouds, panoramio) {
    mapLayersInternal(gmap.mapObject, bike, traffic, weather, clouds, panoramio);
    mapLayersInternal(gphy.mapObject, bike, traffic, weather, clouds, panoramio);
    mapLayersInternal(ghyb.mapObject, bike, traffic, weather, clouds, panoramio);
    mapLayersInternal(gsat.mapObject, bike, traffic, weather, clouds, panoramio);
}

function mapLayersInternal(imap, bike, traffic, weather, clouds, panoramio) {
    debug("mapLayers");
    bikeLayer.setMap(bike ? imap : null);
    trafficLayer.setMap(traffic ? imap : null);
    weatherLayer.setMap(weather ? imap : null);
    cloudLayer.setMap(clouds ? imap : null);
    panoramioLayer.setMap(panoramio ? imap : null);
}

function setMarkerTitle(loc) {
    debug("setMarkerTitle");
    //  marker.setTitle(loc.lat().toFixed(5) + "/" + loc.lon().toFixed(5));
}

function centerAndMark(loc) {
    debug("centerAndMark");
    var lonlat = centerAndMarkOnlyMap(loc);
    startElevator(lonlat);
}

function centerAndMarkOnlyMap(loc) {
    var lonlat = new OpenLayers.LonLat(loc.lng(), loc.lat()).transform(defPro, map.getProjectionObject());
    debug("centerAndMarkOnlyMap: " + lonlat);
    setMarker(lonlat);
    map.setCenter(lonlat);
    return lonlat;
}

function centerAndMarkOnlyMapC(lat, lon) {
    debug("centerAndMarkOnlyMapC");
    var loc = new google.maps.LatLng(lat, lon);
    centerAndMarkOnlyMap(loc);
}

function centerMap(lat, lng) {
    var lonlat = new OpenLayers.LonLat(lng, lat).transform(defPro, map.getProjectionObject());
    debug("centerMap: " + lonlat);
    map.setCenter(lonlat);
}

function moveMarker(lat, lng) {
    var lonlat = new OpenLayers.LonLat(lng, lat).transform(defPro, map.getProjectionObject());
    debug("moveMarker: " + lonlat);
    setMarker(lonlat);
}

function hideMarker() {
    pointFeature.style = style_dark;
    camPos.redraw();
}

function unhideMarker() {
    pointFeature.style = style_orange;
    camPos.redraw();
}


function codeAddressFrom(coordinates, withMap) {
    debug("codeAddressFrom");
    c = coordinates.split(",",2);
    loc = new google.maps.LatLng(c[0],c[1]);
    geocoder.geocode( { 'latLng': loc },
                     function(results, status) {
                         if (status == google.maps.GeocoderStatus.OK) {
                             var erg = results[0].formatted_address;
                             api.set_location(erg);
                             erg += "<br/>";
                             results[0].address_components.foreach( function(k, v) {
                                                                       if (v.types == "country,political") {
                                                                           api.set_country(v.short_name, v.long_name);
                                                                       }
                                                                       if (v.types == "administrative_area_level_1,political") {
                                                                           api.set_state(v.short_name, v.long_name);
                                                                       }
                                                                       if (v.types == "locality,political") {
                                                                           api.set_city(v.short_name, v.long_name);
                                                                       }
                                                                       erg += "<br/><em>" + v.types + ":</em><br/>&nbsp;&nbsp;&nbsp;&nbsp;"
                                                                               + v.short_name;
                                                                       if (v.short_name.localeCompare(v.long_name) !== 0) {
                                                                           erg += " / " + v.long_name;
                                                                       }
                                                                       erg += "<br/>";
                                                                   });
                             api.fillGoogleRaw(erg);
                             api.autoTag(toolsMap);
                         } else {
                             api.alert("Geocode was not successful for the following reason: " + status);
                         }
                     });
}

function codeCoordinatesFrom(address, withMap, region) {
    debug("codeCoordinatesFrom");
    geocoder.geocode( { 'address': address, 'region': region},
                     function(results, status) {
                         if (status == google.maps.GeocoderStatus.OK) {
                             loc = results[0].geometry.location;
                             if (withMap) {
                                 centerAndMark(loc);
                             }
                             startElevator(loc);
                             api.fillGoogleRaw("");
                             api.autoTag(toolsMap);
                         } else {
                             api.alert("Geocode was not successful for the following reason: " + status);
                         }
                     });
}
