# Deploy with Passenger standalone

Follow the same instructions for the Nginx+Passenger, except for the "Nginx site config" step,
run the following command to start Passenger, they need to be run in the directory of github repo
```
bundle exec passenger start
```
and this command to stop Passenger
```
bundle exec passenger stop
```
