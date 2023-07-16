# keyboardFix

The other day, my wired keyboard broke. It held the "9" key whenever nothing was pressed.
Since this was annoying (and I didn't find any working rust crates to differentiate between keyboards),I decided to reach for C.
This abomination is the result. 

## How it works
It listens to all keyDown events (needing privileges) and eats it, if it is a 9 and comes from my wired keyboard. 

It is loosely based on this: https://github.com/mjolnirapp/mjolnir/issues/9 , so thanks for that!

Compile it with 
```shell
gcc -o keyboardSuppressor keyboardSuppressor.c -framework ApplicationServices
```

Run it with 
```shell
./keyboardSuppressor
```
or in verbose mode
```shell
./keyboardSuppressor -v
```
or with a specific keyboard ID to block
```shell
./keyboardSuppressor id
```

### Pros
- I can use my wired keyboard again without much issues!
  
### Cons
- It needs to run in the backgrond
- It only runs on Mac
- It needs admin priviledges
- It only works for my specific problem and doesn't really fix it
- The Keyboard IDs are (semi-)hardcoded, so they might fail on Boot
- It doesn't work on the login screen
- Some other programs that also use a keyboard hook might not realize that the event was eaten

It also is a software solution to a hardware problem, which should never, ever be done in a more or less serious context. 

## Disclaimer
Do not use this. It's probably easier to just buy a new keyboard. 

If you decide to still use it, I can give you advice, but no safety guarantees. Good luck!
