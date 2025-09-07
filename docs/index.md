---
tags:
  - plugin
resource_link: "https://www.redguides.com/community/resources/mq2relocate.1266/"
support_link: "https://www.redguides.com/community/threads/mq2relocate.70204/"
repository: "https://github.com/RedGuides/MQ2Relocate"
authors: "Sic, ChatWithThisName"
tagline: "Easier use of relocation items/aas, as well as specific logic to handle \"converting\""
quick_start: "https://www.youtube.com/watch?v=NF-wGGYol60"
---

# MQ2Relocate

<!--desc-start-->
Plugin to allow easier use of relocation items/aas, as well as specific logic to handle "converting" travel items like Wishing Lamp: and Zueria Slide
<!--desc-end-->

## Commands

<a href="cmd-relocate/">
{% 
  include-markdown "projects/mq2relocate/cmd-relocate.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2relocate/cmd-relocate.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2relocate/cmd-relocate.md') }}

<a href="cmd-translocate/">
{% 
  include-markdown "projects/mq2relocate/cmd-translocate.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2relocate/cmd-translocate.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2relocate/cmd-translocate.md') }}

## Settings

There are no settings to adjust, if the plugin is loaded and you issue a `/relocate` it will do relocate related things.

## Video

- [MQ2Relocate Original Video](https://www.youtube.com/watch?v=NF-wGGYol60)

## Examples

`/relocate pok`
:   This would attempt to relocate you to Plane of Knowledge via: Drunkard's Stein, Brick of Knowledge, The Binden Concerrentia, or The Fabled Binden Concerrentia.

`/relocate lobby`
:   This would attempt to relocate you to the Guild Lobby via Throne of Heroes AA.

`/relocate anchor`
:   This would attempt to relocate you to your Primary Anchor, or if that was unavailable, your Secondary Anchor.

`/relocate anchor1`
:   This would attempt to relocate you to your Primary Anchor specifically.
