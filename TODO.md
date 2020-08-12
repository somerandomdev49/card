# TODO list.

- [x] User-defined functions
- [ ] Arguments, unwrapping input list to arguments, rest.
- [ ] Strings.
- [ ] Macros?
```elixir
function a b =
	@lazy_variable = (a; b; rest;) cons; # variables are lazy.
	.lazy_variable tail tail head
;

main =
	@cond = (function (1; 2; 3;); 3;) eql;
	(.lazy_cond; ["Yes!" print;]; ["Hmm?" print;];) if;
;
```
