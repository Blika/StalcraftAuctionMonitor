# StalcraftAuctionMonitor
Auction monitor for STALCRAFT: X. It is a console app that allows you to send requests to the STALCRAFT: X API (https://eapi.stalcraft.net/reference#/).<br />
It only works for active lots on the auction and its sells history. Only RU region is available, all items are found by their cyrillic names.<br />

# Usage
You need a token ([more info here](https://eapi.stalcraft.net/registration.html)) to get access to the Production API. Otherwise this app will be dealing with Demo API.<br />
This project does not provide you with a token for Production API, you need to get your own.<br />

To start using this app, you need to make sure the database is up-to-date (you need to do it regularly).<br />
You can either download the source files from https://github.com/EXBO-Studio/stalcraft-api.git or use a pull script from this project (you must have `git` installed on your OS).<br />

Afterwards provide the token if you have one, fill empty fields in the config (`data/client.json`). If you do not have the token, you can use the Demo API.<br />

Launch the application, if you provided the token and got authorized you can use the Production API now, otherwise you will be delaing with the Demo API.<br />

# Commands
All commands from below must be typed into this application's console window.<br />

If there was an update to the database (meaning you just pulled it), you need to remap items. Run command `map` in the console, then `init_items`.<br />
Now you have the up-to-date item map to use.<br />

There is a maximum of lots to get from the API. By default it is set to 4 000.<br />
You can change this value by running `offset <number>`.<br />
The bigger this value is, the longer it would take to process everything. Alas this can not be done in multi-threaded way. One request yields a maximum of 200 lots, to process 4 000 lots you will need to send 20 requests. The problem is, if you send requests too frequently, responds cease to contain data you need.<br />

To check active lots run `check <qlt> <ptn> <name>`. `qlt` can be in range from -1 to 5, -1 means any, `ptn` is in range from -1 to 15, -1 means any. `name` is either a cyrillic name or item's id. You can look up these in `data/items/item_map.json`.<br />
This command will return all active lots from the auction.<br />

You can also view sells history for each item. There is a limit for lots, by reaching it the app stops to send requests and process yielded data. It is set to 20 but you are free to change this. Run `lots <number>`.<br />
Run `find <qlt> <ptn> <name>`. Arguments are the same as in `check`. This process will take a while if you set the maximum offset too big. Once there is enough lots to fit the `lots` limit, the process stops.<br />
Now you can see for how much that item was sold and when.<br />

# Build
To build this project yourself, you are going to need `libcurl` and `Boost` installed. There is some trick with `Boost` on Windows - after installation you will need to provide the correct path to the CMakeLists.txt.<br />
Afterwards just run one of the build scripts.
