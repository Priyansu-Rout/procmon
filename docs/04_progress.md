# Progress Log

| Date | Stage | Work done | Issues | Fix / Solution | Commit |
|------|-------|-----------|--------|----------------|--------|
| DD/MM | 1 | Wrote project introduction | None | N/A | docs: add project introduction |
| DD/MM | 2 | Wrote PRD and timeline | None | N/A | docs: add PRD and timeline |
| DD/MM | 3 | Architecture, UML, repo setup | Mermaid syntax errors | Checked on mermaid.live | docs: add architecture and UML |
| DD/MM | 4 | Read /proc, snapshot, CPU % | Names with spaces broke parsing | Parse up to last ")" | feat: read /proc and calculate CPU |
| DD/MM | 4 | Table display, kill command | None | N/A | feat: add display and kill |
| DD/MM | 5 | Sort, filter, renice, user column, logging | gcc warning on %*ld in fscanf | Changed to %*d | feat: add sort, filter, renice, log |
| DD/MM | 5 | Unit tests | False failure: CHECK macro evaluated condition twice | Stored result in a local variable | fix: evaluate test condition once |
| DD/MM | 6 | Final report, slides, demo | None | N/A | docs: add final report |

## Next Steps
- Complete the final presentation and demo
- Tag release v1.0