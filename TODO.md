# TODO list.

* [ ] User-defined functions
* [ ] Lazy evaluaton. (In some sense it already is lazy).
* [ ] Strings
* [ ] Macros
* [ ] Arguments, unwrapping input list to arguments, rest.
```r
function a b =
	@ lazy_variable = (a; b; rest;) cons;
	lazy_variable tail tail head
;

main =
	@ cond = (function (1; 2; 3;); 3;) eql;
	(cond; "Yes!" print; "Hmm?" print;) if;
;
```
