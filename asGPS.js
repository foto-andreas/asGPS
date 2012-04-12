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
      navigationControl: false,
      scaleControl: false,
    };

    map = new google.maps.Map(document.getElementById('map_canvas'),
        myOptions);

    trafficLayer = new google.maps.TrafficLayer();

    bikeLayer = new google.maps.BicyclingLayer();

    weatherLayer = new google.maps.weather.WeatherLayer();

    cloudLayer = new google.maps.weather.CloudLayer();

    marker = new google.maps.Marker({
      position: latLng,
      map: map,
      draggable: true,
      optimized: false,
      icon: new google.maps.MarkerImage("http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|FF3333")
    });

    markerGray = new google.maps.Marker({
      position: latLng,
      map: null,
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
      markerGray.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
      api.marker_moved(e.latLng.lat(),e.latLng.lng(), toolsMap);
    });

    google.maps.event.addListener(marker, 'dragend', function(e) {
      marker.setPosition(e.latLng);
      markerGray.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
      api.marker_moved(e.latLng.lat(),e.latLng.lng(), toolsMap);
    });

    google.maps.event.addListener(markerGray, 'dragend', function(e) {
      markerGray.setPosition(e.latLng);
      marker.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
      api.marker_moved(e.latLng.lat(),e.latLng.lng(), toolsMap);
    });
  }

  function mapLayers(bike, traffic, weather, clouds) {
    bikeLayer.setMap(bike ? map : null);
    trafficLayer.setMap(traffic ? map : null);
    weatherLayer.setMap(weather ? map : null);
    cloudLayer.setMap(clouds ? map : null);
  }

  function setMarkerTitle(loc) {
    marker.setTitle(loc.lat().toFixed(5) + "/" + loc.lng().toFixed(5));
  }

  function centerAndMark(lat, lng) {
    centerAndMarkOnlyMap(lat,lng);
    api.marker_moved(lat, lng, toolsMap);
  }

  function centerAndMarkOnlyMap(lat, lng) {
    loc = new google.maps.LatLng(lat,lng);
    map.setCenter(loc);
    marker.setPosition(loc);
    markerGray.setPosition(loc);
    setMarkerTitle(loc);
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
            centerAndMark(loc.lat(), loc.lng());
        }
        api.fillGoogleRaw("");
        api.autoTag(toolsMap);
      } else {
        api.alert("Geocode was not successful for the following reason: " + status);
      }
    });
  }

  google.maps.event.addDomListener(window, 'load', initialize);
