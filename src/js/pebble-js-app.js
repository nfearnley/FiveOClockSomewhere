Pebble.addEventListener("ready",
    function(e)
    {
        try
        {
            console.log("Ready");
        }
        catch(e)
        {
            console.log("Error: " + e);
        }
    }
);

Pebble.addEventListener("appmessage",
    function(e)
    {
        try
        {
            hours =
            {
                0: "Majuro",
                1: "Sydney",
                2: "Brisbane",
                3: "Tokyo",
                4: "Hong Kong",
                5: "Bangkok",
                6: "Dhaka",
                7: "Karachi",
                8: "Moscow",
                9: "Baghdad",
                10: "Cairo",
                11: "Madrid",
                12: "London",
                13: "Praia",
                14: "Rio de Janeiro",
                15: "Buenos Aires",
                16: "Santo Domingo",
                17: "New York",
                18: "New Orleans",
                19: "Phoenix",
                20: "Tijuana",
                21: "Anchorage",
                22: "Honolulu",
                23: "Pago Pago"
            };
            hour = e.payload.hour;
            name = hours[hour];
            
            var transactionId = Pebble.sendAppMessage({"hour": hour, "name": name},
                function(e)
                {
                    console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
                },
                function(e)
                {
                    console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + e.error.message);
                }
            );
            
            console.log("Appmessage " + hour + ": " + name);
        }
        catch(e)
        {
            console.log("Error: " + e);
        }
    }
);