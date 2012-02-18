  Array.prototype.foreach = function( callback ) {
    for( var k=0; k<this .length; k++ ) {
      callback( k, this[ k ] );
    }
  }

  function initialize() {

    geocoder = new google.maps.Geocoder();

    var latLng = new google.maps.LatLng(0, 0);
    var infowindow = new google.maps.InfoWindow();

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

    marker = new google.maps.Marker({
      position: latLng,
      map: map,
      draggable: true,
      optimized: false
    });

    toolsMap = (gup('toolsMap') == "true") ? true : false;

    google.maps.event.addListener(marker, 'click', function() {
      map.panTo(marker.getPosition());
      api.marker_click(toolsMap);
    });

    google.maps.event.addListener(map, 'click', function(e) {
      marker.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
      api.marker_moved(e.latLng.lat(),e.latLng.lng(), toolsMap);
    });

    google.maps.event.addListener(marker, 'dragend', function(e) {
      marker.setPosition(e.latLng);
      setMarkerTitle(e.latLng);
      api.marker_moved(e.latLng.lat(),e.latLng.lng(), toolsMap);
    });

  }

  function setMarkerTitle(loc) {
     marker.setTitle(loc.lat().toFixed(5) + "/" + loc.lng().toFixed(5));
  }

  function centerAndMark(lat, lng) {
    centerAndMarkOnlyMap(lat,lng);
    api.marker_moved(lat, lng, toolsMap);
  }

  function centerAndMarkOnlyMap(lat, lng) {
    var loc = new google.maps.LatLng(lat,lng);
    map.setCenter(loc);
    marker.setPosition(loc);
    setMarkerTitle(loc);
    api.adjustSize();
  }

  function moveMarker(lat, lng) {
    var loc = new google.maps.LatLng(lat,lng);
    marker.setPosition(loc);
    setMarkerTitle(loc);
  }

  function codeAddressFrom(coordinates, withMap) {
    var c = coordinates.split(",",2);
    var loc = new google.maps.LatLng(c[0],c[1]);
    geocoder.geocode( { 'latLng': loc}, function(results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
        var erg = results[0].formatted_address;
        api.set_location(erg);
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
            erg += "\n" + v.types + v.short_name + "/" + v.long_name;
        });
        api.autoTag(toolsMap);
      } else {
        api.alert("Geocode was not successful for the following reason: " + status);
      }
    });
  }

  function codeCoordinatesFrom(address, withMap) {
   geocoder.geocode( { 'address': address}, function(results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
        var loc = results[0].geometry.location;
        if (withMap) {
            centerAndMark(loc.lat(), loc.lng());
        }
        api.autoTag(toolsMap);
      } else {
        api.alert("Geocode was not successful for the following reason: " + status);
      }
    });
  }

  function gup( name ) {
    name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
    var regexS = "[\\?&]"+name+"=([^&#]*)";
    var regex = new RegExp( regexS );
    var results = regex.exec( window.location.href );
    if ( results == null )
      return "";
    else
      return results[1];
  }

  google.maps.event.addDomListener(window, 'load', initialize);
