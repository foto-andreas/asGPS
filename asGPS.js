  Array.prototype.foreach = function( callback ) {
    for( var k=0; k<this .length; k++ ) {
      callback( k, this[ k ] );
    }
  }

  String.prototype.trim = function () {
    return this.replace(/^\s*/, "").replace(/\s*$/, "");
  }

  function initialize() {

    geocoder = new google.maps.Geocoder();

    elevator = new google.maps.ElevationService();

    var latLng = new google.maps.LatLng(0, 0);
    var infowindow = new google.maps.InfoWindow();

    var region = gup( "region" );

    var myOptions = {
      zoom: 15,
      center: latLng,
      draggable: true,
      mapTypeId: google.maps.MapTypeId.ROADMAP,
      mapTypeControlOptions: {
        mapTypeIds: [
            google.maps.MapTypeId.HYBRID,
            google.maps.MapTypeId.ROADMAP,
            google.maps.MapTypeId.SATELLITE,
            google.maps.MapTypeId.TERRAIN ],
        style: google.maps.MapTypeControlStyle.DROPDOWN_MENU,
        position: google.maps.ControlPosition.TOP_RIGHT
      },
      panControl: false,
      streetViewControl: false,
      navigationControl: true,
      scaleControl: true,
      scrollwheel: true

    };

    map = new google.maps.Map(document.getElementById('map_canvas'),
        myOptions);

    trafficLayer = new google.maps.TrafficLayer();

    bikeLayer = new google.maps.BicyclingLayer();

    weatherLayer = new google.maps.weather.WeatherLayer();

    cloudLayer = new google.maps.weather.CloudLayer();

    panoramioLayer = new google.maps.panoramio.PanoramioLayer();

    marker = new google.maps.Marker({
      position: latLng,
      map: null,
      draggable: true,
      optimized: false,
      icon: new google.maps.MarkerImage("http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|FF3333")
    });

    markerGray = new google.maps.Marker({
      position: latLng,
      map: map,
      draggable: true,
      optimized: false,
      icon: new google.maps.MarkerImage("http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|909090")
    });

    toolsMap = (gup('toolsMap') == "true") ? true : false;

    google.maps.event.addListener(marker, 'click', function() {
      map.panTo(marker.getPosition());
      api.marker_click(toolsMap);
    });

    google.maps.event.addListener(markerGray, 'click', function() {
      map.panTo(marker.getPosition());
      api.marker_click(toolsMap);
    });

    google.maps.event.addListener(map, 'click', function(e) {
      marker.setPosition(e.latLng);
      startElevator(e.latLng);
      markerGray.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
      api.marker_moved(e.latLng.lat(),e.latLng.lng(), toolsMap);
    });

    google.maps.event.addListener(marker, 'dragend', function(e) {
      marker.setPosition(e.latLng);
      startElevator(e.latLng);
      markerGray.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
    });

    google.maps.event.addListener(markerGray, 'dragend', function(e) {
      markerGray.setPosition(e.latLng);
      startElevator(e.latLng);
      marker.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
    });
  }

  function startElevator(loc) {
    locs = [];
    locs.push(loc);
    positionalRequest = {
        'locations': locs
    }
    elevator.getElevationForLocations(positionalRequest, function(results, status) {
        height = 0;
        if (status == google.maps.ElevationStatus.OK) {
          if (results[0]) {
            height = results[0].elevation;
          } else {
            // api.alert("No results found");
          }
        } else {
          // api.alert("Elevation service failed due to: " + status);
        }
        api.marker_moved(loc.lat(), loc.lng(), height, toolsMap);
      });
  }

  function mapLayers(bike, traffic, weather, clouds, panoramio) {
    bikeLayer.setMap(bike ? map : null);
    trafficLayer.setMap(traffic ? map : null);
    weatherLayer.setMap(weather ? map : null);
    cloudLayer.setMap(clouds ? map : null);
    panoramioLayer.setMap(panoramio ? map : null);
  }

  function setMarkerTitle(loc) {
    marker.setTitle(loc.lat().toFixed(5) + "/" + loc.lng().toFixed(5));
  }

  function centerAndMark(loc) {
    centerAndMarkOnlyMap(loc);
    startElevator(loc);
  }

  function centerAndMarkOnlyMap(loc) {
    map.setCenter(loc);
    marker.setPosition(loc);
    markerGray.setPosition(loc);
    setMarkerTitle(loc);
  }

  function centerAndMarkOnlyMapC(lat, lng) {
    loc = new google.maps.LatLng(lat,lng);
    centerAndMarkOnlyMap(loc);
  }

  function centerMap(lat, lng) {
    loc = new google.maps.LatLng(lat,lng);
    map.setCenter(loc);
  }

  function moveMarker(lat, lng) {
    loc = new google.maps.LatLng(lat,lng);
    marker.setPosition(loc);
    markerGray.setPosition(loc);
    setMarkerTitle(loc);
  }

  function hideMarker() {
    marker.setMap(null);
    markerGray.setMap(map);
  }

  function unhideMarker() {
    marker.setMap(map);
    markerGray.setMap(null);
  }

  function codeAddressFrom(coordinates, withMap) {
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

  google.maps.event.addDomListener(window, 'load', initialize);
