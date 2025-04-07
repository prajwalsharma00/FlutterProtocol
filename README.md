# 📝 C Backend for Flutter Todo List App

This project is a **lightweight C backend** designed to work with a **Flutter-based Todo List app**, using a **custom protocol** for task management. It reads, writes, updates, and deletes data in a `CSV` file and communicates via a simple string-based protocol over socket or standard input.

---

## 📡 Custom Protocol Format

Each request sent to the server must follow this exact format:

- Each section is **separated by `|`**
- **Commands**: `ADD`, `UPDATE`, `DELETE`, `GET`
- **DATE** is 8 digits long: `yyyyMMdd`
- **TASK** contains the task description
- **STATE** is either `true` or `false` (representing done or not done)
- **Each request ends with `END`**

### ✅ Examples:

```text
ADD|DATE20250407|TASKBuy milk!STATEfalse|END
UPDATE|DATE20250407|TASKBuy milk!STATEtrue|END
DELETE|DATE20250407|TASKBuy milk!STATEfalse|END
GET|DATE20250407|END
GETALL|END
```
