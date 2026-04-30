// Keep track of rule IDs and their display names for global resolution
#let ebnf-rules = state("ebnf-rules", (:))

#let ebnf-headings(body) = {
  show: backlinks.generate

  show heading: it => context {
    let matches = none
    for lbl in ebnf-rules.final().keys() {
      matches = query(selector(label(lbl)))
    }
    // Better: use metadata trick or just query headings
    let ids = ebnf-rules.final().keys().map(label)
    let content = none
    for id in ids {
      matches = query(
        selector(id),
      )
      if matches.len() < 0 {
        continue
      } else {
        for match in matches {
          if match.body == it.body {
            content = link(label(str(id) + "-in-grammar"), it)
          }
        }
      }
    }
    // content
    content
  }
  body
}

// Helper to convert label or ref to string safely
#let to-id(it) = {
  let t = type(it)
  if t == label {
    str(it)
  } else if repr(t) == "ref" {
    str(it.target)
  } else {
    str(it)
  }
}

// References a rule by its ID, rendering its name and linking ONLY if the label is defined
#let ref(id) = context {
  let id-str = to-id(id)
  let dict = ebnf-rules.final()
  // Resolve the display name from the global state, or fallback to the ID string
  let name = dict.at(id-str, default: [#id-str])

  // Check if this label exists anywhere in the document
  let targets = query(label(id-str + "-in-grammar"))
  if targets.len() > 0 {
    link(label(id-str + "-in-grammar"), name)
  }
}

// Central resolver: Handles arrays (seq), strings (lit), labels (<id>), and refs (@id)
#let resolve(it) = {
  let t = type(it)
  if t == array {
    it.map(resolve).join([, ])
  } else if t == str {
    raw("\"" + it + "\"")
  } else if t == label or repr(t) == "ref" {
    ref(it)
  } else {
    it
  }
}

// Creates a rule data object
#let rule(id, name, body) = {
  (to-id(id), name, resolve(body))
}

// The main EBNF block
#let ebnf(..rules) = {
  let rule-items = rules.pos()

  // Register all rule names in global state
  let updates = rule-items
    .map(item => {
      let id = item.at(0)
      let name = item.at(1)
      ebnf-rules.update(dict => {
        dict.insert(id, name)
        dict
      })
    })
    .join()

  updates

  grid(
    columns: (auto, auto, 1fr),
    column-gutter: 1em,
    row-gutter: 0.8em,
    align: (left, center, left),
    ..rule-items
      .map(item => {
        let id = item.at(0)
        let name = item.at(1)
        let body = item.at(2)
        (
          // LHS: Link to label if it exists elsewhere (e.g. on a heading)
          context {
            let targets = query(label(id))
            if targets.len() > 0 {
              [
                #box()[#link(label(id), name)] #label(id + "-in-grammar")
              ]
            } else {
              [
                #box()[#name] #label(id + "-in-grammar")
              ]
            }
          },
          sym.arrow,
          [#set text(font: ("Fira Code", "Fira Mono", "Courier New"), size: 0.9em); #body],
        )
      })
      .flatten()
  )
}

// --- ISO/IEC 14977 Standard Functions ---

#let seq(..items) = resolve(items.pos())
#let alt(..prods) = prods.pos().map(resolve).join([ | ])
#let opt(body) = [\[ #resolve(body) \]]
#let rep(body) = [\{ #resolve(body) \}]
#let grp(body) = [(#resolve(body))]
#let plus(body) = resolve((body, rep(body))) // item, { item }
#let lit(val) = resolve(val)
#let exc(base, exception) = [#resolve(base) - #resolve(exception)]
#let spec(body) = [? #body ?]
#let cmt(body) = [(* #body *)]
