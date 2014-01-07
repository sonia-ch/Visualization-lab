#ifndef IVW_SERIALIZER_TEST_H
#define IVW_SERIALIZER_TEST_H


#include <modules/unittests/unittestsmoduledefine.h>

#include <inviwo/core/io/serialization/ivwserializable.h>

#define SERIALITION_FILE_NAME InviwoApplication::getPtr()->getPath(InviwoApplication::PATH_MODULES,"unittests/tmpfiles/serilizationtests.xml")

TEST(SerialitionTest,initTest){
    std::string filename = SERIALITION_FILE_NAME;
    IvwSerializer serializer(filename);
    EXPECT_TRUE(true);
}

template <typename T>
T serializationOfType(T inValue){
    std::string filename = SERIALITION_FILE_NAME;
    IvwSerializer serializer(filename);

    serializer.serialize("serializedValue",inValue);


    serializer.writeFile();

    IvwDeserializer deserializer(filename);
    deserializer.readFile();

    T outValue;
    deserializer.deserialize("serializedValue",outValue);

    return outValue;
}

#define TYPE_TEST(n,T,v) TEST(SerialitionTest,n##TypeTest){EXPECT_NEAR(T(v),serializationOfType(T(v)),std::numeric_limits<T>::epsilon());} 
#define MIN_TEST(n,T) TEST(SerialitionTest,n##MinTest){EXPECT_NEAR(std::numeric_limits<T>::min(),serializationOfType(std::numeric_limits<T>::min()),std::numeric_limits<T>::epsilon());}
#define MAX_TEST(n,T) TEST(SerialitionTest,n##MaxTest){EXPECT_NEAR(std::numeric_limits<T>::max(),serializationOfType(std::numeric_limits<T>::max()),std::numeric_limits<T>::epsilon());}
#define EPSILON_TEST(n,T) TEST(SerialitionTest,n##EpsilonTest){EXPECT_NEAR(std::numeric_limits<T>::epsilon(),serializationOfType(std::numeric_limits<T>::epsilon()),std::numeric_limits<T>::epsilon());}

#define NUMERIC_TESTS(n,T,v) TYPE_TEST(n,T,v) MIN_TEST(n,T)  MAX_TEST(n,T) EPSILON_TEST(n,T) 

static const float oneMinusEpsilonF = 1.0f-std::numeric_limits<float>::epsilon();
static const double oneMinusEpsilonD = 1.0-std::numeric_limits<double>::epsilon();

NUMERIC_TESTS(floatSerialization,float,3.14f);
NUMERIC_TESTS(doubleSerializationTest,double,6.28);

NUMERIC_TESTS(int8SerializationTest,int8_t,3);
NUMERIC_TESTS(int16SerializationTest,int16_t,6);
NUMERIC_TESTS(int32SerializationTest,int32_t,654);
//NUMERIC_TESTS(int64SerializationTest,int64_t,6789);

NUMERIC_TESTS(uint8SerializationTest,uint8_t,3);
NUMERIC_TESTS(uint16SerializationTest,uint16_t,6);
NUMERIC_TESTS(uint32SerializationTest,uint32_t,654);
//NUMERIC_TESTS(uint64SerializationTest,uint64_t,6789);


NUMERIC_TESTS(intLeast8SerializationTest,int_least8_t,3);
NUMERIC_TESTS(intLeast16SerializationTest,int_least16_t,6);
NUMERIC_TESTS(intLeast32SerializationTest,int_least32_t,654);
//NUMERIC_TESTS(intLeast64SerializationTest,int_least64_t,6789);

NUMERIC_TESTS(uintLeast8SerializationTest,uint_least8_t,3);
NUMERIC_TESTS(uintLeast16SerializationTest,uint_least16_t,6);
NUMERIC_TESTS(uintLeast32SerializationTest,uint_least32_t,654);
//NUMERIC_TESTS(uintLeast64SerializationTest,uint_least64_t,6789);



NUMERIC_TESTS(intFast8SerializationTest,int_fast8_t,3);
NUMERIC_TESTS(intFast16SerializationTest,int_fast16_t,6);
NUMERIC_TESTS(intFast32SerializationTest,int_fast32_t,654);
//NUMERIC_TESTS(intFast64SerializationTest,int_fast64_t,6789);

NUMERIC_TESTS(uintFast8SerializationTest,uint_fast8_t,3);
NUMERIC_TESTS(uintFast16SerializationTest,uint_fast16_t,6);
NUMERIC_TESTS(uintFast32SerializationTest,uint_fast32_t,654);
//NUMERIC_TESTS(uintFast64SerializationTest,uint_fast64_t,6789);

TYPE_TEST(oneMinusEpsilonFloatTest,float,oneMinusEpsilonF);
TYPE_TEST(oneMinusEpsilonDobuleTest,double,oneMinusEpsilonD);


class IVW_MODULE_UNITTESTS_API MinimumSerilizableClass : public IvwSerializable{
public:
    MinimumSerilizableClass(float v=0): value_(v) {}


    virtual void serialize( IvwSerializer &s ) const {
        s.serialize("classVariable",value_);
    }

    virtual void deserialize( IvwDeserializer &d ) {
        d.deserialize("classVariable",value_);
    }

    bool operator==(const MinimumSerilizableClass &v)const{
        return value_ == v.value_;
    }

public:
    float value_;


};

TEST(SerialitionTest,IvwSerializableClassTest){
    MinimumSerilizableClass inValue(12),outValue;

    std::string filename = SERIALITION_FILE_NAME;
    IvwSerializer serializer(filename);

    serializer.serialize("serializedValue",inValue);


    serializer.writeFile();

    IvwDeserializer deserializer(filename);
    deserializer.readFile();

    deserializer.deserialize("serializedValue",outValue);

    EXPECT_EQ(inValue.value_,12);
    EXPECT_NE(outValue.value_,0);
    EXPECT_EQ(inValue,outValue);
}


TEST(SerialitionTest,IvwSerializableClassAsPointerTest){
    MinimumSerilizableClass *inValue = new MinimumSerilizableClass(12),*outValue = 0;

    std::string filename = SERIALITION_FILE_NAME;
    IvwSerializer serializer(filename);

    serializer.serialize("serializedValue",inValue);


    serializer.writeFile();

    IvwDeserializer deserializer(filename);
    deserializer.readFile();

    deserializer.deserialize("serializedValue",outValue);

    EXPECT_EQ(inValue->value_,12);
    EXPECT_NE(outValue->value_,0);
    EXPECT_EQ(inValue,outValue);
}

TEST(SerialitionTest,floatVectorTest){
    std::vector<float> inVector,outVector;
    inVector.push_back(0.1f);
    inVector.push_back(0.2f);
    inVector.push_back(0.3f);

    std::string filename = SERIALITION_FILE_NAME;
    IvwSerializer serializer(filename);

    serializer.serialize("serializedVector",inVector,"value");

    serializer.writeFile();

    IvwDeserializer deserializer(filename);
    deserializer.readFile();
//    deserializer.deserialize("serializedVector",outVector,"value");

    ASSERT_EQ(inVector.size(),outVector.size());
    for(size_t i = 0;i<inVector.size();i++){
        EXPECT_EQ(inVector[i],outVector[i]);
    }
}





TEST(SerialitionTest,vectorOfNonPointersTest){
    std::vector<MinimumSerilizableClass> inVector,outVector;
    inVector.push_back(MinimumSerilizableClass(0.1f));
    inVector.push_back(MinimumSerilizableClass(0.2f));
    inVector.push_back(MinimumSerilizableClass(0.3f));

    std::string filename = SERIALITION_FILE_NAME;
    IvwSerializer serializer(filename);

    serializer.serialize("serializedVector",inVector,"value");


    serializer.writeFile();

    IvwDeserializer deserializer(filename);
    deserializer.readFile();
//    deserializer.deserialize("serializedVector",outVector,"value");

    ASSERT_EQ(inVector.size(),outVector.size());
    for(size_t i = 0;i<inVector.size();i++){
        EXPECT_EQ(inVector[i],outVector[i]);
    }
}


TEST(SerialitionTest,vectorOfPointersTest){
    std::vector<MinimumSerilizableClass*> inVector,outVector;
    inVector.push_back(new MinimumSerilizableClass(0.1f));
    inVector.push_back(new MinimumSerilizableClass(0.2f));
    inVector.push_back(new MinimumSerilizableClass(0.3f));

    std::string filename = SERIALITION_FILE_NAME;
    IvwSerializer serializer(filename);

    serializer.serialize("serializedVector",inVector,"value");


    serializer.writeFile();

    IvwDeserializer deserializer(filename);
    deserializer.readFile();
    deserializer.deserialize("serializedVector",outVector,"value");

    ASSERT_EQ(inVector.size(),outVector.size());
    for(size_t i = 0;i<inVector.size();i++){
        EXPECT_EQ(inVector[i]->value_,outVector[i]->value_);
    }
}

TEST(SerialitionTest,vec2Tests){
    vec2 inVec(1.1f,2.2f),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
}

TEST(SerialitionTest,vec3Tests){
    vec3 inVec(1.1f,2.2f,3.3f),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
}

TEST(SerialitionTest,vec4Tests){
    vec4 inVec(1.1f,2.2f,3.3f,4.4f),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
    EXPECT_EQ(inVec.w,outVec.w);
}


TEST(SerialitionTest,ivec2Tests){
    ivec2 inVec(1,2),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
}

TEST(SerialitionTest,ivec3Tests){
    ivec3 inVec(1,2,3),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
}

TEST(SerialitionTest,ivec4Tests){
    ivec4 inVec(1,2,3,4),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
    EXPECT_EQ(inVec.w,outVec.w);
}



TEST(SerialitionTest,uvec2Tests){
    uvec2 inVec(1,2),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
}

TEST(SerialitionTest,uvec3Tests){
    uvec3 inVec(1,2,3),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
}

TEST(SerialitionTest,uvec4Tests){
    uvec4 inVec(1,2,3,4),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
    EXPECT_EQ(inVec.w,outVec.w);
}



TEST(SerialitionTest,bvec2Tests){
    bvec2 inVec(false,true),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
}

TEST(SerialitionTest,bvec3Tests){
    bvec3 inVec(false,true,false),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
}

TEST(SerialitionTest,bvec4Tests){
    bvec4 inVec(false,true,false,true),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
    EXPECT_EQ(inVec.w,outVec.w);
}


TEST(SerialitionTest,dvec2Tests){
    dvec2 inVec(1.1,2.2),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
}

TEST(SerialitionTest,dvec3Tests){
    dvec3 inVec(1.1,2.2,3.3),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
}

TEST(SerialitionTest,dvec4Tests){
    dvec4 inVec(1.1,2.2,3.3,4.4),outVec;
    outVec = serializationOfType(inVec);
    EXPECT_EQ(inVec.x,outVec.x);
    EXPECT_EQ(inVec.y,outVec.y);
    EXPECT_EQ(inVec.z,outVec.z);
    EXPECT_EQ(inVec.w,outVec.w);
}


TEST(SerialitionTest,mat2Tests){
    mat2 inMat(0.0f,1.0f,2.0f,3.0f),outMat;
    outMat = serializationOfType(inMat);
    int s = 2;
    for(int i = 0;i<s;i++)for(int j = 0;j<s;j++)
        EXPECT_EQ(inMat[i][j],outMat[i][j]);
}

TEST(SerialitionTest,mat3Tests){
    mat3 inMat(0.0f,1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f),outMat;
    outMat = serializationOfType(inMat);
    int s = 3;
    for(int i = 0;i<s;i++)for(int j = 0;j<s;j++)
        EXPECT_EQ(inMat[i][j],outMat[i][j]);
}

TEST(SerialitionTest,mat4Tests){
    mat4 inMat(0.0f,1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f,10.0f,11.0f,12.0f,13.0f,14.0f,15.0f),outMat;
    outMat = serializationOfType(inMat);
    int s = 4;
    for(int i = 0;i<s;i++)for(int j = 0;j<s;j++)
        EXPECT_EQ(inMat[i][j],outMat[i][j]);
}

#endif