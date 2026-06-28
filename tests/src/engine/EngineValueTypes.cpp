#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <AngelScriptWrapperTests/TestRefType.hpp>
#include <datetime.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <scriptdictionary.h>
#include <scripthandle.h>
#include <scripthelper.h>
#include <scriptmathcomplex.h>
#include <weakref.h>

using namespace testing;

namespace pod {
struct[[= as::RefType]] Ref {};

struct Empty {};

struct AlsoEmpty : Empty {};

struct WithPtr {
    void* p;
};

struct EmptyWithPtr : private WithPtr {};

struct EmptyWithProtectedPtr : protected WithPtr {};

struct EmptyWithPublicPtr : public WithPtr {};

struct WithRef {
    int& i;
};

struct WithMoveRef {
    int&& i;
};

struct WithPrimitives {
    bool f;
    int i;
    double d;
};

struct WithArray {
    int i[7];
};

struct WithPrivatePtr {
private:
    void* p;
};

struct WithProtectedPtr {
protected:
    void* p;
};

struct Nested {
    Empty e;
    int w;
    WithArray a;

    union {
        int r;
        bool y;
    };

    struct {
        int f;
    } instance;

    struct Named {
        int f;
    } instance2;

    struct {
        struct {
            double t;
        } inner;
    } instance3;
};

struct BadNestedStruct {
    struct {
        void* p;
    } instance;
};

struct BadNestedNamedStruct {
    struct Named {
        void* p;
    } instance;
};

struct IgnoreNonInstancedStructs {
    struct Named {
        void* p;
    };
};

struct Multi : Nested, private AlsoEmpty {};

struct BadMulti : Nested, private EmptyWithPtr {};
} // namespace pod

TEST(AngelScriptEngineValueTypes, PodChecking) {
    std::unordered_set<std::type_index> podTypes;

    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::WithPrimitives>(podTypes));
    podTypes.insert(std::type_index(typeid(bool)));
    podTypes.insert(std::type_index(typeid(int)));
    podTypes.insert(std::type_index(typeid(double)));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^pod::WithPrimitives>(podTypes));

    podTypes.insert(std::type_index(typeid(pod::Empty)));
    podTypes.insert(std::type_index(typeid(pod::WithArray)));

    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::Ref>(podTypes));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^pod::Empty>(podTypes));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^pod::AlsoEmpty>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::WithPtr>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::EmptyWithPtr>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::EmptyWithProtectedPtr>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::EmptyWithPublicPtr>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::WithRef>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::WithMoveRef>(podTypes));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^pod::WithPrimitives>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::WithArray>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::WithPrivatePtr>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::WithProtectedPtr>(podTypes));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^pod::Nested>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::BadNestedStruct>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::BadNestedNamedStruct>(podTypes));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^pod::IgnoreNonInstancedStructs>(podTypes));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^pod::Multi>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^pod::BadMulti>(podTypes));

    podTypes.insert(std::type_index(typeid(std::chrono::system_clock::time_point)));
    podTypes.insert(std::type_index(typeid(float)));
    podTypes.insert(std::type_index(typeid(AS_NAMESPACE_QUALIFIER asINT64)));

    EXPECT_TRUE(as::IsAngelScriptPodType<^^AS_NAMESPACE_QUALIFIER CDateTime>(podTypes));
    EXPECT_TRUE(as::IsAngelScriptPodType<^^AS_NAMESPACE_QUALIFIER Complex>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^AS_NAMESPACE_QUALIFIER CScriptDictValue>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^AS_NAMESPACE_QUALIFIER CScriptHandle>(podTypes));
    EXPECT_FALSE(as::IsAngelScriptPodType<^^AS_NAMESPACE_QUALIFIER CScriptWeakRef>(podTypes));
}

struct Vector {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct VectorCollection {
    bool flag;
    Vector vector;
    const Vector constVector;

    void init() {
        vector.x = 1.2;
        vector.y = 3.4;
        vector.z = 5.6;
    }

    double mult() {
        return vector.x * vector.y * vector.z;
    }
};

TEST(AngelScriptEngineValueTypes, PodTypes) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterObjectType<^^Vector>(), 0);
    ASSERT_GE(engine.RegisterObjectType<^^VectorCollection>(), 0);
    ASSERT_GE(engine.RegisterObjectType<^^as::TestRefType>(), 0);

    EXPECT_TRUE(engine.IsPodType<bool>());
    EXPECT_TRUE(engine.IsPodType<std::int8_t>());
    EXPECT_TRUE(engine.IsPodType<std::uint8_t>());
    EXPECT_TRUE(engine.IsPodType<std::int16_t>());
    EXPECT_TRUE(engine.IsPodType<std::uint16_t>());
    EXPECT_TRUE(engine.IsPodType<std::int32_t>());
    EXPECT_TRUE(engine.IsPodType<std::uint32_t>());
    EXPECT_TRUE(engine.IsPodType<std::int64_t>());
    EXPECT_TRUE(engine.IsPodType<std::uint64_t>());
    EXPECT_TRUE(engine.IsPodType<float>());
    EXPECT_TRUE(engine.IsPodType<double>());
    EXPECT_TRUE(engine.IsPodType<Vector>());
    EXPECT_TRUE(engine.IsPodType<VectorCollection>());
    EXPECT_FALSE(engine.IsPodType<as::TestRefType>());

    double res = 0.0;

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "Vector v; v.x = 4.5; v.y = 6.7; v.z = 8.9; return v.x * v.y * v.z;", &res, 11
        ),
        0
    );
    EXPECT_DOUBLE_EQ(res, 268.335);

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "VectorCollection v; v.init(); return v.mult();", &res, 11),
        0
    );
    EXPECT_DOUBLE_EQ(res, 22.848);
}

struct NonPodType {
    inline NonPodType(const double val = 0.0) {
        p = new double(val);
    }

    inline ~NonPodType() noexcept {
        if (p) { delete p; }
    }

    static inline void Constructor[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT), = as::ObjFirst ]](
        void* memory
    ) {
        new (memory) NonPodType();
    }

    static inline void Constructor[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT), = as::ObjFirst ]](
        void* memory, const double val
    ) {
        new (memory) NonPodType(val);
    }

    static unsigned int destructorCallCount;

    static inline void Destructor[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_DESTRUCT), = as::ObjFirst ]](
        void* memory
    ) {
        static_cast<NonPodType*>(memory)->~NonPodType();
        ++destructorCallCount;
    }

    NonPodType& opAssign(NonPodType const& o) {
        if (o.p && p) {
            *p = *o.p;
        } else if (p) {
            *p = 0.0;
        }
        return *this;
    }

    double* p;
};

unsigned int NonPodType::destructorCallCount = 0;

NonPodType retByValue(NonPodType cpy, NonPodType const& i) {
    NonPodType n(cpy.p ? *cpy.p : 0.0);

    if ((cpy.p == i.p) && n.p) { *n.p = -1.0; }

    return n;
}

void retByRef(NonPodType& o, NonPodType const& i) {
    if (o.p && i.p) { *o.p = *i.p; }
}

TEST(AngelScriptEngineValueTypes, ConstructorsAndDestructor) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterObjectType<^^NonPodType>(), 0);
    EXPECT_FALSE(engine.IsPodType<NonPodType>());

    ASSERT_GE(engine.RegisterGlobalFunction<^^retByValue>(), 0);
    ASSERT_GE(engine.RegisterGlobalFunction<^^retByRef>(), 0);

    double res = 0.0;

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "NonPodType n; n.p = 268.335; return n.p;", &res, 11), 0
    );
    EXPECT_DOUBLE_EQ(res, 268.335);

    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "NonPodType n(22.848); return n.p;", &res, 11), 0);
    EXPECT_DOUBLE_EQ(res, 22.848);

    EXPECT_EQ(NonPodType::destructorCallCount, 2);

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "NonPodType n(22.848); return retByValue(n, n).p;", &res, 11
        ),
        0
    );
    EXPECT_DOUBLE_EQ(res, 22.848);

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "NonPodType n(22.848), o; retByRef(o, n); return o.p;", &res, 11
        ),
        0
    );
    EXPECT_DOUBLE_EQ(res, 22.848);
}
