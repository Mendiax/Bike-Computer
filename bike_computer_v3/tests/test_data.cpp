const char *forecast =
    "{\"latitude\":52.52,\"longitude\":13.419998,\"generationtime_ms\":3.094911575317383,\"utc_offset_seconds\":7200,\"timezone\":\"Europe/Berlin\","
    "\"timezone_abbreviation\":\"CEST\",\"elevation\":38.0,\"hourly_units\":{\"time\":\"iso8601\",\"temperature_2m\":\"°C\",\"pressure_msl\":\"hPa\","
    "\"precipitation\":\"mm\",\"windspeed_10m\":\"km/h\",\"winddirection_10m\":\"°\",\"windgusts_10m\":\"km/h\"},\"hourly\":{\"time\":[\"2022-08-15T00:00\","
    "\"2022-08-15T01:00\",\"2022-08-15T02:00\",\"2022-08-15T03:00\",\"2022-08-15T04:00\",\"2022-08-15T05:00\",\"2022-08-15T06:00\",\"2022-08-15T07:00\","
    "\"2022-08-15T08:00\",\"2022-08-15T09:00\",\"2022-08-15T10:00\",\"2022-08-15T11:00\",\"2022-08-15T12:00\",\"2022-08-15T13:00\",\"2022-08-15T14:00\","
    "\"2022-08-15T15:00\",\"2022-08-15T16:00\",\"2022-08-15T17:00\",\"2022-08-15T18:00\",\"2022-08-15T19:00\",\"2022-08-15T20:00\",\"2022-08-15T21:00\","
    "\"2022-08-15T22:00\",\"2022-08-15T23:00\"],\"temperature_2m\":[24.0,23.3,22.8,22.0,21.2,20.5,20.2,20.4,21.4,23.0,24.9,26.4,27.6,28.4,29.4,30.0,30.0,"
    "30.2,29.9,23.2,20.5,20.6,19.9,20.5],\"pressure_msl\":[1007.2,1007.2,1007.1,1007.0,1007.3,1006.4,1006.3,1006.5,1006.3,1005.7,1005.4,1005.1,1004.7,"
    "1004.4,1003.6,1003.1,1003.0,1002.1,1002.2,1004.4,1003.7,1005.0,1005.9,1006.4],\"precipitation\":[0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,"
    "0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.1,0.1,0.2,0.0],\"windspeed_10m\":[7.2,8.6,5.7,7.2,6.1,6.3,7.2,8.5,8.9,7.9,9.5,10.8,11.9,13.3,11.2,12.6,10.9,10.1,"
    "10.6,17.6,9.1,13.0,6.4,3.4],\"winddirection_10m\":[93.0,90.0,108.0,117.0,118.0,103.0,90.0,102.0,111.0,114.0,101.0,111.0,125.0,123.0,132.0,127.0,136.0,"
    "117.0,114.0,224.0,263.0,304.0,74.0,108.0],\"windgusts_10m\":[14.8,17.6,18.0,15.5,15.1,15.5,14.8,18.7,21.2,20.2,23.0,26.3,28.4,31.0,32.0,29.9,29.9,26.6,"
    "30.2,71.3,38.5,27.7,29.2,14.0]},\"daily_units\":{\"time\":\"iso8601\",\"sunrise\":\"iso8601\",\"sunset\":\"iso8601\",\"winddirection_10m_dominant\":\"°\"},"
    "\"daily\":{\"time\":[\"2022-08-15\"],\"sunrise\":[\"2022-08-15T05:47\"],\"sunset\":[\"2022-08-15T20:34\"],\"winddirection_10m_dominant\":[119.71125]}}";

const char *day1_json =
    "{\"latitude\":52.25,\"longitude\":21.0,\"generationtime_ms\":0.6890296936035156,\"utc_offset_seconds\":7200,\"timezone\":\"Europe/Berlin\","
    "\"timezone_abbreviation\":\"CEST\",\"elevation\":113.0,\"current_weather\":{\"temperature\":26.5,\"windspeed\":9.8,\"winddirection\":126.0,"
    "\"weathercode\":3.0,\"time\":\"2022-08-19T21:00\"},\"hourly_units\":{\"time\":\"iso8601\",\"temperature_2m\":\"°C\",\"pressure_msl\":\"hPa\","
    "\"precipitation\":\"mm\",\"windspeed_10m\":\"km/h\",\"winddirection_10m\":\"°\",\"windgusts_10m\":\"km/h\"},\"hourly\":{\"time\":[\"2022-08-19T00:00\","
    "\"2022-08-19T01:00\",\"2022-08-19T02:00\",\"2022-08-19T03:00\",\"2022-08-19T04:00\",\"2022-08-19T05:00\",\"2022-08-19T06:00\",\"2022-08-19T07:00\","
    "\"2022-08-19T08:00\",\"2022-08-19T09:00\",\"2022-08-19T10:00\",\"2022-08-19T11:00\",\"2022-08-19T12:00\",\"2022-08-19T13:00\",\"2022-08-19T14:00\","
    "   \"2022-08-19T15:00\",    \"2022-08-19T16:00\",    \"2022-08-19T17:00\",    \"2022-08-19T18:00\",    \"2022-08-19T19:00\",    \"2022-08-19T20:00\","
    "    \"2022-08-19T21:00\",    \"2022-08-19T22:00\",    \"2022-08-19T23:00\"],\"temperature_2m\" : [    22.5,    21.9,    21.8,    21.4,    21.1,    20.8,"
    "    20.6,    21.4,    23.2,    25.3,    27.3,    28.7,    29.9,    30.8,    31.4,    31.8,    31.8,    31.6,    30.8,    29.7,    28.0,    26.5,    25.5,"
    "    24.7],\"pressure_msl\" : [    1013.4,    1013.0,    1013.0,    1012.6,    1012.6,    1012.3,    1012.2,    1012.3,    1012.5,    1012.5,    1012.8,"
    "    1012.4,    1012.4,    1011.9,    1012.4,    1011.9,    1011.1,    1010.9,    1011.0,    1011.5,    1011.8,    1012.2,    1012.2,    1012.4],"
    "\"precipitation\" : [    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,"
    "    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0,    0.0],\"windspeed_10m\" : [    5.2,    5.5,    5.9,    5.9,    5.4,    6.1,"
    "    7.2,    8.4,    9.5,    10.9,    12.2,    15.8,    16.6,    16.6,    15.3,    15.5,    15.2,    15.0,    14.3,    11.2,    9.6,    9.8,"
    "    9.8,    9.3],\"winddirection_10m\" : [    115.0,    122.0,    128.0,    128.0,    127.0,    118.0,    117.0,    115.0,    115.0,    117.0,"
    "    124.0,    139.0,    139.0,    141.0,    141.0,    144.0,    144.0,    145.0,    146.0,    135.0,    124.0,    126.0,    126.0,    126.0],"
    "\"windgusts_10m\" : [    10.8,    12.2,    12.2,    13.3,    12.6,    13.3,    15.8,    18.7,    22.7,    27.4,    30.2,    38.2,    40.3,"
    "    41.0,    41.8,    39.2,    37.4,    36.7,    35.6,    33.1,    25.9,    22.3,    23.0,    22.7] }, \"daily_units\" : {\"time\" : \"iso8601\","
    "\"sunrise\" : \"iso8601\",\"sunset\" : \"iso8601\",\"winddirection_10m_dominant\" : \"°\" }, \"daily\" : {\"time\" : [\"2022-08-19\"],\"sunrise\""
    " : [\"2022-08-19T05:24\"],\"sunset\" : [\"2022-08-19T19:54\"],\"winddirection_10m_dominant\" : [132.33426] }    }";

const char *test_json =
    "{\"latitude\":0.0,\"longitude\":0.0,\"generationtime_ms\":0.45692920684814453,\"utc_offset_seconds\":7200,\"timezone\":\"Europe/Berlin\",\"timezone_abbreviation\":"
    "\"CEST\",\"elevation\":0.0,\"current_weather\":{\"temperature\":23.0,\"windspeed\":23.1,\"winddirection\":203.0,\"weathercode\":2.0,\"time\":\"2022-08-23T19:00\"},"
    "\"hourly_units\":{\"time\":\"iso8601\",\"temperature_2m\":\"°C\",\"pressure_msl\":\"hPa\",\"precipitation\":\"mm\",\"windspeed_10m\":\"km/h\",\"winddirection_10m\":"
    "\"°\",\"windgusts_10m\":\"km/h\"},\"hourly\":{\"time\":[\"2022-08-23T00:00\",\"2022-08-23T01:00\",\"2022-08-23T02:00\",\"2022-08-23T03:00\",\"2022-08-23T04:00\","
    "\"2022-08-23T05:00\",\"2022-08-23T06:00\",\"2022-08-23T07:00\",\"2022-08-23T08:00\",\"2022-08-23T09:00\",\"2022-08-23T10:00\",\"2022-08-23T11:00\",\"2022-08-23T12:00\","
    "\"2022-08-23T13:00\",\"2022-08-23T14:00\",\"2022-08-23T15:00\",\"2022-08-23T16:00\",\"2022-08-23T17:00\",\"2022-08-23T18:00\",\"2022-08-23T19:00\",\"2022-08-23T20:00\","
    "\"2022-08-23T21:00\",\"2022-08-23T22:00\",\"2022-08-23T23:00\"],\"temperature_2m\":[23.1,23.2,23.3,23.0,23.0,23.1,23.1,23.0,23.1,22.9,23.0,23.1,23.1,23.1,23.2,23.1,23.0,"
    "23.1,23.1,23.0,23.1,23.1,23.2,23.2],\"pressure_msl\":[1014.0,1013.8,1012.7,1012.5,1011.8,1011.7,1011.8,1012.7,1013.0,1014.0,1014.9,1015.1,1015.1,1014.5,1013.8,1013.3,1012.7,"
    "1012.6,1012.5,1012.7,1013.2,1013.7,1014.0,1014.5],\"precipitation\":[0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.1,0.1,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],"
    "\"windspeed_10m\":[27.2,27.3,29.0,29.7,30.9,29.6,27.3,26.2,26.8,25.7,23.8,20.5,21.6,22.8,24.5,25.5,24.8,24.3,23.4,23.1,22.7,22.6,23.3,23.3],\"winddirection_10m\""
    ":[195.0,193.0,187.0,186.0,187.0,190.0,189.0,189.0,186.0,186.0,183.0,181.0,183.0,185.0,194.0,196.0,199.0,201.0,203.0,203.0,203.0,202.0,202.0,197.0],\"windgusts_10m\""
    ":[36.0,35.6,35.6,38.2,40.0,39.6,38.2,35.3,34.6,34.9,33.5,31.3,28.8,29.9,31.7,33.5,33.1,32.4,31.7,30.6,30.2,29.9,30.6,31.0]},\"daily_units\":{\"time\":\"iso8601\","
    "\"sunrise\":\"iso8601\",\"sunset\":\"iso8601\",\"winddirection_10m_dominant\":\"°\"},\"daily\":{\"time\":[\"2022-08-23\"],\"sunrise\":[\"2022-08-23T07:58\"],"
    "\"sunset\":[\"2022-08-23T20:07\"],\"winddirection_10m_dominant\":[192.30557]}}";