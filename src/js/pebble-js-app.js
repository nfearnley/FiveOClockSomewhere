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
    function (e)
    {
        hours =
        {0: ""},
        
        console.log("Appmessage: " + e.payload);
    }
);
