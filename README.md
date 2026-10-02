# Hệ thống Quản lý Khóa học Trực tuyến (Online Course Enrollment & Certification)

PRF193 – Topic 05

## 1. Cách build & chạy

```bash
make        # biên dịch, sinh ra file thực thi course_app
./course_app
```

Hoặc biên dịch tay:
```bash
g++ -std=c++17 -o course_app src/*.cpp
```

Chương trình tự động đọc `courses.dat` và `enrollments.dat` khi khởi động
(nếu file chưa tồn tại, chương trình bắt đầu với danh sách rỗng), và tự
động lưu lại khi bạn chọn "Lưu & Thoát" ở menu chính.

## 2. Kiến trúc & Class Diagram (mô tả bằng chữ)

```
Course (abstract base)
 ├── FreeCourse    : đạt chứng chỉ khi progress >= 100%
 └── PaidCourse    : đạt chứng chỉ khi progress >= 100% VÀ finalScore >= 50

Enrollment
 └── liên kết learner <-> courseId, lưu progress/score/trạng thái chứng chỉ

CourseManager
 ├── vector<shared_ptr<Course>>       courses
 ├── vector<shared_ptr<Enrollment>>   enrollments
 ├── map<string,int>                  enrollmentCountByCourse   (STL Map)
 └── template<T> searchBy / sortBy    (dùng chung cho Course lẫn Enrollment)

FileManager
 └── đọc/ghi courses.dat & enrollments.dat (định dạng text, phân tách bằng '|')

Exceptions.h
 ├── InvalidInputException  (dữ liệu sai định dạng/khoảng giá trị)
 ├── DuplicateException     (ID trùng, đăng ký trùng)
 └── NotFoundException      (không tìm thấy ID)

main.cpp: menu console, gọi CourseManager + FileManager
```

## 3. Ánh xạ với 4 yêu cầu kỹ thuật của đề bài

| Yêu cầu | Được hiện thực ở đâu |
|---|---|
| **OOP** (Encapsulation, Inheritance, Polymorphism) | `Course` là lớp trừu tượng, `FreeCourse`/`PaidCourse` kế thừa và override `isEligibleForCertificate()` — đây là điểm polymorphism chính, quyết định điều kiện cấp chứng chỉ khác nhau |
| **STL** | `std::vector<shared_ptr<T>>` lưu danh sách; `std::map<string,int>` đếm số học viên/khóa học; `std::sort` trong template `sortBy` |
| **Memory Management** | Toàn bộ Course/Enrollment được quản lý bằng `std::shared_ptr`, không có `new`/`delete` thủ công → không rò rỉ bộ nhớ |
| **File Processing** | `FileManager::loadData()` gọi lúc khởi động, `saveData()` gọi lúc thoát |
| **Exception Handling** | 3 exception tùy chỉnh, được `catch` ngay tại menu và in thông báo lỗi thân thiện, chương trình không bao giờ crash vì input sai |
| **Template functions** | `CourseManager::searchBy<T>` và `sortBy<T>` dùng chung cho cả Course và Enrollment (Pattern Recognition — tái sử dụng 1 đoạn code cho 2 kiểu dữ liệu khác nhau) |

## 4. Test case đã kiểm thử

- Thêm khóa học Free và Paid → hiển thị đúng loại, đúng học phí
- Đăng ký học viên vào khóa học không tồn tại → `NotFoundException`
- Đăng ký trùng learner + course → `DuplicateException`
- Cập nhật tiến độ ngoài khoảng 0–100 → `InvalidInputException`
- Cập nhật đủ điều kiện (Free: progress=100; Paid: progress=100 & score>=50)
  → cờ `certificateIssued` tự động bật thành `Issued`
- Lưu file, khởi động lại chương trình → dữ liệu được khôi phục đầy đủ,
  bộ đếm ID (CRSxxx/ENRxxx) tiếp tục tăng đúng từ số lớn nhất đã lưu

## 5. Gợi ý mở rộng nếu muốn nâng điểm thêm

- Thêm lớp `AuditLog` ghi lại lịch sử thay đổi (phục vụ AI Audit Log của báo cáo)
- Thêm ràng buộc: không cho enroll vào khóa học đã đóng
- Vẽ flowchart cho hàm `updateProgress()` — đây là hàm có nhánh rẽ rõ nhất
  (rẽ theo loại khóa học → rẽ theo điều kiện đạt chứng chỉ)
